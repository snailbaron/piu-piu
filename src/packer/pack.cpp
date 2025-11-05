#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include <data_generated.h>
#include <yaml-cpp/yaml.h>

namespace fb = data::fb;

namespace {

template <std::integral T>
std::vector<T> readFile(const std::filesystem::path& path)
{
    static_assert(sizeof(T) == 1, "readFile<T>: T must have size 1");

    auto stream = std::ifstream{path, std::ios::binary | std::ios::ate};
    stream.exceptions(std::ios::badbit | std::ios::failbit);

    auto len = stream.tellg();
    stream.seekg(0);

    auto data = std::vector<T>(len);
    stream.read(reinterpret_cast<char*>(data.data()), len);
    stream.close();

    return data;
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "usage: pack SHEET META OUTPUT\n";
        return EXIT_FAILURE;
    }
    auto sheetPath = std::filesystem::path{argv[1]};
    auto metaPath = std::filesystem::path{argv[2]};
    auto outputPath = std::filesystem::path{argv[3]};

    auto meta = YAML::LoadFile(metaPath);

    auto fbBuilder = flatbuffers::FlatBufferBuilder{};

    auto sheet = readFile<uint8_t>(sheetPath);
    auto text = std::string{};
    auto sheetFrames = std::vector<fb::SheetFrame>{};
    auto animations = std::vector<fb::Animation>{};
    auto sprites = std::vector<fb::Sprite>{};

    auto appendText = [&text](const std::string& t) -> fb::Range
    {
        uint32_t l = text.length();
        text += t;
        uint32_t r = text.length();
        return fb::Range{l, r};
    };

    for (const auto& spriteMeta : meta["sprites"]) {
        auto fbSpriteName = appendText(spriteMeta["name"].as<std::string>());
        auto fbSpriteSize = fb::Size{
            spriteMeta["size"]["w"].as<uint32_t>(),
            spriteMeta["size"]["h"].as<uint32_t>(),
        };

        uint32_t animationRangeBegin = animations.size();
        for (const auto& animationMeta : spriteMeta["animations"]) {
            auto fbAnimationName =
                appendText(animationMeta["name"].as<std::string>());

            uint32_t frameRangeBegin = sheetFrames.size();
            for (const auto& frameMeta : animationMeta["frames"]) {
                sheetFrames.emplace_back(
                    frameMeta["x"].as<uint32_t>(),
                    frameMeta["y"].as<uint32_t>(),
                    frameMeta["w"].as<uint32_t>(),
                    frameMeta["h"].as<uint32_t>(),
                    frameMeta["duration"].as<uint32_t>());
            }
            uint32_t frameRangeEnd = sheetFrames.size();

            auto fbFrameRange = fb::Range{frameRangeBegin, frameRangeEnd};

            animations.emplace_back(fbAnimationName, fbFrameRange);
        }
        uint32_t animationRangeEnd = animations.size();
        auto fbAnimationRange =
            fb::Range{animationRangeBegin, animationRangeEnd};

        sprites.emplace_back(fbSpriteName, fbSpriteSize, fbAnimationRange);
    }

    auto fbSheet = fbBuilder.CreateVector(sheet);
    auto fbText = fbBuilder.CreateString(text);
    auto fbSheetFrames = fbBuilder.CreateVectorOfStructs(sheetFrames);
    auto fbAnimations = fbBuilder.CreateVectorOfStructs(animations);
    auto fbSprites = fbBuilder.CreateVectorOfStructs(sprites);

    auto fbData = fb::CreateData(
        fbBuilder, fbSheet, fbText, fbSheetFrames, fbAnimations, fbSprites);

    fbBuilder.Finish(fbData, "PIUF");

    auto out = std::ofstream{outputPath, std::ios::binary};
    out.exceptions(std::ios::badbit | std::ios::failbit);
    out.write(
        reinterpret_cast<char*>(fbBuilder.GetBufferPointer()),
        fbBuilder.GetSize());
    out.close();
}
