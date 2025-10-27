#include "timer.hpp"

#include <sdlxx.hpp>

int main()
{
    auto sdl = sdl::Init{SDL_INIT_AUDIO | SDL_INIT_VIDEO};

    auto window = sdl::Window{"Piu-Piu", 800, 600, SDL_WINDOW_FULLSCREEN};
    auto renderer = sdl::Renderer{window};

    auto timer = FrameTimer{60};
    bool done = false;
    for (;;) {
        for (SDL_Event e; SDL_PollEvent(&e);) {
            if (e.type == SDL_EVENT_QUIT) {
                done = true;
                break;
            }
        }

        if (done) {
            break;
        }

        if (auto framesPassed = timer(); framesPassed > 0) {
            renderer.setDrawColor(30, 30, 30);
            renderer.clear();
            renderer.present();
        }

        timer.relax();
    }
}
