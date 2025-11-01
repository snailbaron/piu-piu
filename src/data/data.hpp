#pragma once

#include <cstddef>
#include <filesystem>
#include <span>

#include <x/mmap.hpp>

namespace data {

class Data {
public:
    Data(const std::filesystem::path& path);

    std::span<std::byte> sheet() const;

private:
};

} // namespace data
