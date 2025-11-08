#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <yaml-cpp/yaml.h>

#include <data.hpp>

namespace {

[[maybe_unused]] void writeFile(
    std::span<const std::byte> data, const std::filesystem::path& path)
{
    auto out = std::ofstream{path, std::ios::binary};
    out.exceptions(std::ios::badbit | std::ios::failbit);
    out.write(
        reinterpret_cast<const char*>(data.data()),
        static_cast<std::streamsize>(data.size()));
    out.close();
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "usage: unpack DATA OUTPUT_DIR\n";
        return EXIT_FAILURE;
    }
    auto dataPath = std::filesystem::path{argv[1]};
    auto outputDir = std::filesystem::path{argv[2]};

    auto mmap = x::Mmap{dataPath};

    auto root = data::Data{dataPath};

    std::filesystem::create_directory(outputDir);
    writeFile(root.sheet(), outputDir / "sheet.png");

    auto yaml = YAML::Emitter{};
    yaml << YAML::BeginMap << YAML::Key << "sprites" << YAML::Value
         << YAML::BeginSeq;
    for (const auto& sprite : root.sprites()) {
        yaml << YAML::BeginMap;

        yaml << YAML::Key << "name" << YAML::Value
             << std::string{sprite.name()};
        yaml << YAML::Key << "size" << YAML::Value << YAML::BeginMap
             << YAML::Key << "w" << YAML::Value << sprite.size().w()
             << YAML::Key << "h" << YAML::Value << sprite.size().h()
             << YAML::EndMap;

        yaml << YAML::Key << "animations" << YAML::Value << YAML::BeginSeq;
        for (const auto& animation : sprite.animations()) {
            yaml << YAML::BeginMap;

            yaml << YAML::Key << "name" << YAML::Value
                 << std::string{animation.name()};

            yaml << YAML::Key << "frames" << YAML::Value << YAML::BeginSeq;
            for (const auto& frame : animation.frames()) {
                yaml << YAML::BeginMap;

                yaml << YAML::Key << "x" << YAML::Value << frame.x();
                yaml << YAML::Key << "y" << YAML::Value << frame.y();
                yaml << YAML::Key << "w" << YAML::Value << frame.w();
                yaml << YAML::Key << "h" << YAML::Value << frame.h();
                yaml << YAML::Key << "duration" << YAML::Value
                     << frame.duration();

                yaml << YAML::EndMap;
            }
            yaml << YAML::EndSeq;

            yaml << YAML::EndMap;
        }
        yaml << YAML::EndSeq;

        yaml << YAML::EndMap;
    }
    yaml << YAML::EndSeq;

    auto outMeta = std::ofstream{outputDir / "meta.yaml"};
    outMeta.exceptions(std::ios::badbit | std::ios::failbit);
    outMeta << yaml.c_str();
    outMeta.close();
}
