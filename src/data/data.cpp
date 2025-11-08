#include "data.hpp"

namespace data {

namespace {

std::string_view text(const fb::Data* fbData, const fb::Range& fbTextRange)
{
    return fbData->text()->string_view().substr(
        fbTextRange.begin(), fbTextRange.end() - fbTextRange.begin());
}

} // namespace

std::string_view Animation::name() const
{
    return text(_fbData, _fbAnimation->name());
}

std::span<const fb::SheetFrame> Animation::frames() const
{
    const auto* base = reinterpret_cast<const fb::SheetFrame*>(
        _fbData->sheet_frames()->Data());
    auto size =
        _fbAnimation->frame_range().end() - _fbAnimation->frame_range().begin();

    return {base + _fbAnimation->frame_range().begin(), size};
}

Animation::Animation(const fb::Data* fbData, const fb::Animation* fbAnimation)
    : _fbData(fbData)
    , _fbAnimation(fbAnimation)
{ }

std::string_view Sprite::name() const
{
    return _fbData->text()->string_view().substr(
        _fbSprite->name().begin(),
        _fbSprite->name().end() - _fbSprite->name().begin());
}

const fb::Size& Sprite::size() const
{
    return _fbSprite->size();
}

Range<fb::Animation, Animation> Sprite::animations() const
{
    return {_fbData, _fbData->animations(), _fbSprite->animation_range()};
}

Sprite::Sprite(const fb::Data* fbData, const fb::Sprite* fbSprite)
    : _fbData(fbData)
    , _fbSprite(fbSprite)
{ }

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

Range<fb::Sprite, Sprite> Data::sprites() const
{
    return {_fbData, _fbData->sprites()};
}

} // namespace data
