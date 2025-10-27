#pragma once

#include <memory>

namespace sdl {

template <class T, void (*deleter)(T*)>
class Wrapper {
public:
    Wrapper(T* ptr)
    {
        _ptr.reset(ptr);
    }

    operator const T*() const
    {
        return _ptr.get();
    }

    operator T*()
    {
        return _ptr.get();
    }

protected:
    const T* ptr() const
    {
        return _ptr.get();
    }

    T* ptr()
    {
        return _ptr.get();
    }

private:
    std::unique_ptr<T, void (*)(T*)> _ptr{nullptr, deleter};
};

} // namespace sdl
