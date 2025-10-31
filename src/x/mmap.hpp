#pragma once

#include <filesystem>

namespace x {

class Mmap {
public:
    Mmap(const std::filesystem::path& path);
    ~Mmap();

    Mmap(Mmap&& other) noexcept;
    Mmap& operator=(Mmap&& other) noexcept;

    Mmap(const Mmap&) = delete;
    Mmap& operator=(const Mmap&) = delete;

    void open(const std::filesystem::path& path);
    void close() noexcept;

    friend void swap(Mmap& lhs, Mmap& rhs) noexcept;

private:
    void* _addr = nullptr;
    size_t _len = 0;
};

} // namespace x
