#include "sdlxx.hpp"

#include "error.hpp"

#include <mutex>

namespace sdl {

namespace {

std::mutex initMutex;
bool initialized = false;

} // namespace

Init::Init(SDL_InitFlags flags)
{
    auto lock = std::lock_guard{initMutex};

    if (initialized) {
        throw Error{"sdlxx is initialized twice"};
    }
    initialized = true;

    check(SDL_Init(flags));
}

Init::~Init()
{
    auto lock = std::lock_guard{initMutex};

    SDL_Quit();
    initialized = false;
}

Window::Window(const std::string& title, int w, int h, SDL_WindowFlags flags)
    : Wrapper(check(SDL_CreateWindow(title.c_str(), w, h, flags)))
{ }

Renderer::Renderer(Window& window)
    : Wrapper(check(SDL_CreateRenderer(window, nullptr)))
{ }

Renderer::Renderer(Window& window, const std::string& name)
    : Wrapper(check(SDL_CreateRenderer(window, name.c_str())))
{ }

void Renderer::present()
{
    check(SDL_RenderPresent(ptr()));
}

void Renderer::clear()
{
    check(SDL_RenderClear(ptr()));
}

void Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    check(SDL_SetRenderDrawColor(ptr(), r, g, b, a));
}

} // namespace sdl
