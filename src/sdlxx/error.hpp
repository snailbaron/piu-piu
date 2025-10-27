#pragma once

#include <exception>
#include <source_location>
#include <string>
#include <string_view>

namespace sdl {

class Error : public std::exception {
public:
    Error(
        std::string_view message,
        std::source_location sl = std::source_location::current());
    explicit Error(std::source_location sl);

    const char* what() const noexcept override;

private:
    std::string _message;
};

void check(
    bool success, std::source_location sl = std::source_location::current());

template <class T>
T* check(T* ptr, std::source_location sl = std::source_location::current())
{
    if (ptr == nullptr) {
        throw Error{sl};
    }
    return ptr;
}

} // namespace sdl
