#pragma once

#include <chrono>

class FrameTimer {
public:
    FrameTimer(int fps);

    int operator()();
    double delta() const;
    void relax() const;

private:
    using Clock = std::chrono::high_resolution_clock;

    double _delta = 0.0;
    Clock::duration _frameDuration;
    Clock::time_point _start;
    long long _lastFrame = 0;
};
