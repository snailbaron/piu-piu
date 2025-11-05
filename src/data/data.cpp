#include "data.hpp"

namespace data {

Data::Data(const std::filesystem::path& path)
    : _mmap{path}
    , _fbData(fb::GetData(_mmap.ptr()))
{ }

std::span<const std::byte> Data::sheet() const
{
    return std::span<const std::byte>{
        reinterpret_cast<const std::byte*>(_fbData->sheet()->data()),
        _fbData->sheet()->size()};
}

} // namespace data
