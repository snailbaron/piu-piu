#pragma once

#include "wrapper.hpp"

#include <SDL3/SDL.h>

#include <cstdint>
#include <string>

namespace sdl {

class Init {
public:
    explicit Init(SDL_InitFlags flags);
    ~Init();

    Init(const Init&) = delete;
    Init(Init&&) = delete;
    Init& operator=(const Init&) = delete;
    Init& operator=(Init&&) = delete;
};

class Window : public Wrapper<SDL_Window, SDL_DestroyWindow> {
public:
    Window(const std::string& title, int w, int h, SDL_WindowFlags flags);
};

class Renderer : public Wrapper<SDL_Renderer, SDL_DestroyRenderer> {
public:
    explicit Renderer(Window& window);
    Renderer(Window& window, const std::string& name);

    void present();
    void clear();
    void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
};

} // namespace sdl
