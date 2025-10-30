#pragma once

#include <exception>
#include <source_location>
#include <string>
#include <string_view>

namespace x {

class Error : public std::exception {
public:
    Error(
        std::string_view message,
        std::source_location sl = std::source_location::current());

    const char* what() const noexcept override;

private:
    std::string _message;
};

} // namespace x
