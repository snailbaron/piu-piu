#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include <data_generated.h>
#include <yaml-cpp/yaml.h>

std::vector<char> readFile(const std::filesystem::path& path)
{
    auto stream = std::ifstream{path, std::ios::binary | std::ios::ate};
    stream.exceptions(std::ios::badbit | std::ios::failbit);

    auto len = stream.tellg();
    stream.seekg(0);

    auto data = std::vector<char>(len);
    stream.read(data.data(), len);
    stream.close();

    return data;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "usage: pack SHEET META\n";
        return EXIT_FAILURE;
    }
    auto sheetPath = std::filesystem::path{argv[1]};
    auto metaPath = std::filesystem::path{argv[2]};

    auto meta = YAML::LoadFile(metaPath);

    auto fbBuilder = flatbuffers::FlatBufferBuilder{};

    auto sheet = readFile(sheetPath);
    auto text = std::string{};
    auto sheetFrames = std::vector<data::SheetFrame>{};
    auto animations = std::vector<data::Animation>{};
    auto sprites = std::vector<data::Sprite>{};

    auto appendText = [&text](const std::string& t) -> data::Range
    {
        uint32_t l = text.length();
        text += t;
        uint32_t r = text.length();
        return data::Range{l, r};
    };

    for (const auto& spriteMeta : meta["sprites"]) {
        auto fbSpriteName = appendText(spriteMeta["name"].as<std::string>());
        auto fbSpriteSize = data::Size{
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

            auto fbFrameRange = data::Range{frameRangeBegin, frameRangeEnd};

            animations.emplace_back(fbAnimationName, fbFrameRange);
        }
        uint32_t animationRangeEnd = animations.size();
        auto fbAnimationRange =
            data::Range{animationRangeBegin, animationRangeEnd};

        sprites.emplace_back(fbSpriteName, fbSpriteSize, fbAnimationRange);
    }
}
