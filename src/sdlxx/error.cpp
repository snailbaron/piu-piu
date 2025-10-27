#include "error.hpp"

#include <SDL3/SDL.h>

#include <format>

namespace sdl {

Error::Error(std::string_view message, std::source_location sl)
    : _message(
          std::format(
              "{}:{}:{} ({}): {}",
              sl.file_name(),
              sl.line(),
              sl.column(),
              sl.function_name(),
              message))
{ }

Error::Error(std::source_location sl)
    : Error{SDL_GetError(), sl}
{ }

const char* Error::what() const noexcept
{
    return _message.c_str();
}

void check(bool success, std::source_location sl)
{
    if (!success) {
        throw Error{sl};
    }
}

} // namespace sdl
