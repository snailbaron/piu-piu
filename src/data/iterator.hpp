#pragma once

#include <cstddef>
#include <iterator>

#include <flatbuffers/flatbuffers.h>

namespace data {

template <class T>
class Iterator {
    static_assert(std::random_access_iterator<Iterator<T>>);

public:
    using difference_type = ptrdiff_t;
    using value_type = T;

    Iterator(const flatbuffers::Vector<const T*>* fbContainer, ptrdiff_t index)
        : _fbContainer(fbContainer)
        , _index(index)
    { }

    const T& operator*() const
    {
        return *_fbContainer->Get(_index);
    }

    const T& operator[](ptrdiff_t n) const
    {
        return *_fbContainer->Get(_index + n);
    }

    Iterator& operator++()
    {
        _index++;
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
        _index += n;
        return *this;
    }

    Iterator& operator--()
    {
        _index--;
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
        _index -= n;
        return *this;
    }

    ptrdiff_t operator-(const Iterator& other) const
    {
        return _index - other._index;
    }

    auto operator<=>(const Iterator&) const = default;

private:
    const flatbuffers::Vector<const T*>* _fbContainer = nullptr;
    ptrdiff_t _index = 0;
};

template <class T>
Iterator<T> operator+(Iterator<T> it, ptrdiff_t n)
{
    it += n;
    return it;
}

template <class T>
Iterator<T> operator+(ptrdiff_t n, Iterator<T> it)
{
    return it + n;
}

template <class T>
Iterator<T> operator-(Iterator<T> it, ptrdiff_t n)
{
    it -= n;
    return it;
}

template <class T>
class Range {
public:
    explicit Range(
        const flatbuffers::Vector<const T*>* fbContainer,
        size_t begin,
        size_t end)
        : _fbContainer(fbContainer)
        , _begin(begin)
        , _end(end)
    { }

    Iterator<T> begin() const
    {
        return {_fbContainer, _begin};
    }

    Iterator<T> end() const
    {
        return {_fbContainer, _end};
    }

private:
    const flatbuffers::Vector<const T*>* _fbContainer = nullptr;
    size_t _begin = 0;
    size_t _end = 0;
};

} // namespace data
