#pragma once

#include <cstddef>
#include <filesystem>
#include <span>

#include <x/mmap.hpp>

#include <data_generated.h>

#include "iterator.hpp"

namespace data {

class Animation {
public:
    std::string_view name() const
    {
        return _fbData->text()->string_view().substr(
            _fbAnimation->name().begin(),
            _fbAnimation->name().end() - _fbAnimation->name().begin());
    }

    Range<fb::SheetFrame> frames() const
    {
        return Range{
            _fbData->sheet_frames(),
            _fbAnimation->frame_range().begin(),
            _fbAnimation->frame_range().end()};
    }

private:
    const fb::Data* _fbData = nullptr;
    const fb::Animation* _fbAnimation = nullptr;
};

class Sprite {
public:
    std::string_view name() const
    {
        return _fbData->text()->string_view().substr(
            _fbSprite->name().begin(),
            _fbSprite->name().end() - _fbSprite->name().begin());
    }

    const fb::Size& size() const
    {
        return _fbSprite->size();
    }

    Range<fb::Animation> animations() const
    {
        return Range{
            _fbData->animations(),
            _fbSprite->animation_range().begin(),
            _fbSprite->animation_range().end()};
    }

private:
    const fb::Data* _fbData = nullptr;
    const fb::Sprite* _fbSprite = nullptr;
};

class Data {
public:
    Data(const std::filesystem::path& path);

    std::span<const std::byte> sheet() const;

    Range<fb::Sprite> sprites() const
    {
        return Range{_fbData->sprites(), 0, _fbData->sprites()->size()};
    }

private:
    x::Mmap _mmap;
    const fb::Data* _fbData = nullptr;
};

} // namespace data
