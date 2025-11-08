#pragma once

#include <cstddef>
#include <filesystem>
#include <format>
#include <iostream>
#include <iterator>
#include <span>

#include <x/mmap.hpp>

#include <data_generated.h>

#include "iterator.hpp"

namespace data {

class Animation {
public:
    std::string_view name() const;
    std::span<const fb::SheetFrame> frames() const;

private:
    friend class Iterator<fb::Animation, Animation>;

    Animation(const fb::Data* fbData, const fb::Animation* fbAnimation);

    const fb::Data* _fbData = nullptr;
    const fb::Animation* _fbAnimation = nullptr;
};

class Sprite {
public:
    std::string_view name() const;
    const fb::Size& size() const;
    Range<fb::Animation, Animation> animations() const;

private:
    friend class Iterator<fb::Sprite, Sprite>;

    Sprite(const fb::Data* fbData, const fb::Sprite* fbSprite);

    const fb::Data* _fbData = nullptr;
    const fb::Sprite* _fbSprite = nullptr;
};

class Data {
public:
    Data(const std::filesystem::path& path);
    std::span<const std::byte> sheet() const;
    Range<fb::Sprite, Sprite> sprites() const;

private:
    x::Mmap _mmap;
    const fb::Data* _fbData = nullptr;
};

} // namespace data
