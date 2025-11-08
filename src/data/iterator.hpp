#pragma once

#include <cstddef>
#include <iostream>
#include <iterator>

#include <flatbuffers/flatbuffers.h>

#include <data_generated.h>

namespace data {

template <class Fb, class Wrapper>
class Iterator {
    // static_assert(std::random_access_iterator<Iterator<T>>);

public:
    using difference_type = ptrdiff_t;
    using value_type = Wrapper;

    Iterator(const fb::Data* fbData, const Fb* ptr)
        : _fbData(fbData)
        , _ptr(ptr)
    { }

    Wrapper operator*() const
    {
        return Wrapper{_fbData, _ptr};
    }

    Wrapper operator[](ptrdiff_t n) const
    {
        return Wrapper{_fbData, _ptr + n};
    }

    Iterator& operator++()
    {
        _ptr++;
        return *this;
    }

    Iterator operator++(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    Iterator& operator+=(ptrdiff_t n)
    {
        _ptr += n;
        return *this;
    }

    Iterator& operator--()
    {
        _ptr--;
        return *this;
    }

    Iterator operator--(int)
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    Iterator& operator-=(ptrdiff_t n)
    {
        _ptr -= n;
        return *this;
    }

    ptrdiff_t operator-(const Iterator& other) const
    {
        return _ptr - other._ptr;
    }

    auto operator<=>(const Iterator&) const = default;

private:
    const fb::Data* _fbData = nullptr;
    const Fb* _ptr = nullptr;
};

template <class Fb, class Wrapper>
Iterator<Fb, Wrapper> operator+(Iterator<Fb, Wrapper> it, ptrdiff_t n)
{
    it += n;
    return it;
}

template <class Fb, class Wrapper>
Iterator<Fb, Wrapper> operator+(ptrdiff_t n, Iterator<Fb, Wrapper> it)
{
    return it + n;
}

template <class Fb, class Wrapper>
Iterator<Fb, Wrapper> operator-(Iterator<Fb, Wrapper> it, ptrdiff_t n)
{
    it -= n;
    return it;
}

template <class Fb, class Wrapper>
class Range {
public:
    Range(const fb::Data* fbData, const flatbuffers::Vector<const Fb*>* vector)
        : _fbData(fbData)
        , _begin(reinterpret_cast<const Fb*>(vector->Data()))
        , _end(reinterpret_cast<const Fb*>(vector->Data()) + vector->size())
    { }

    Range(
        const fb::Data* fbData,
        const flatbuffers::Vector<const Fb*>* vector,
        const fb::Range& range)
        : _fbData(fbData)
        , _begin(reinterpret_cast<const Fb*>(vector->Data()) + range.begin())
        , _end(reinterpret_cast<const Fb*>(vector->Data()) + range.end())
    { }

    Iterator<Fb, Wrapper> begin() const
    {
        return {_fbData, _begin};
    }

    Iterator<Fb, Wrapper> end() const
    {
        return {_fbData, _end};
    }

private:
    const fb::Data* _fbData = nullptr;
    const Fb* _begin = nullptr;
    const Fb* _end = nullptr;
};

} // namespace data
