#include <data_generated.h>

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace {

std::vector<std::byte> readFile(const std::filesystem::path& path)
{
    auto stream = std::ifstream{path, std::ios::binary | std::ios::ate};
    auto fileSize = stream.tellg();
    stream.seekg(0);

    auto data = std::vector<std::byte>(fileSize);
    stream.read(reinterpret_cast<char*>(data.data()), fileSize);
    stream.close();

    return data;
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "usage: packer SHEET DATA\n";
        return EXIT_FAILURE;
    }

    auto sheetPath = std::filesystem::path{argv[1]};
    auto dataPath = std::filesystem::path{argv[2]};

    auto builder = flatbuffers::FlatBufferBuilder{};
    auto sheet = readFile(sheetPath);
    auto sheetVector = builder.CreateVector(sheet);

    auto framesVector = builder.CreateVectorOfStructs();

    data::CreateData(builder, sheetVector);
}
