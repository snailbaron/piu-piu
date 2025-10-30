#include "error.hpp"

#include <format>

namespace x {

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

const char* Error::what() const noexcept
{
    return _message.c_str();
}

} // namespace x
