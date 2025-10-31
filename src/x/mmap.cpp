#include "mmap.hpp"

#include "error.hpp"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <filesystem>
#include <format>
#include <source_location>
#include <utility>

namespace x {

namespace {

void throwErrno(std::source_location sl = std::source_location::current())
{
    const int e = errno;
    throw Error{
        std::format("{}: {}", strerrorname_np(e), strerrordesc_np(e)), sl};
}

} // namespace

Mmap::Mmap(const std::filesystem::path& path)
{
    open(path);
}

Mmap::~Mmap()
{
    close();
}

Mmap::Mmap(Mmap&& other) noexcept
{
    swap(*this, other);
}

Mmap& Mmap::operator=(Mmap&& other) noexcept
{
    if (this != &other) {
        close();
        swap(*this, other);
    }
    return *this;
}

void Mmap::open(const std::filesystem::path& path)
{
    const int fd = ::open(path.string().c_str(), O_RDONLY);
    if (fd == -1) {
        throwErrno();
    }

    struct stat sb{};
    if (::fstat(fd, &sb) == -1) {
        ::close(fd);
        throwErrno();
    }

    void* addr = ::mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        ::close(fd);
        throwErrno();
    }

    _addr = addr;
    _len = sb.st_size;

    ::close(fd);
}

void Mmap::close() noexcept
{
    if (_addr != nullptr) {
        ::munmap(_addr, _len);
        _addr = nullptr;
        _len = 0;
    }
}

void swap(Mmap& lhs, Mmap& rhs) noexcept
{
    std::swap(lhs._addr, rhs._addr);
    std::swap(lhs._len, rhs._len);
}

} // namespace x
