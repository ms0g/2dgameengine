#pragma once

#include <SDL2/SDL.h>

struct AnimationComponent {
    int numFrames;
    int currentFrame;
    int frameSpeedRate;
    bool shouldLoop;
    uint32_t startTime;

    explicit AnimationComponent(int numFrames = 1, int frameSpeedRate = 1, bool shouldLoop = true) :
            numFrames(numFrames),
            frameSpeedRate(frameSpeedRate),
            shouldLoop(shouldLoop),
            currentFrame(1),
            startTime(SDL_GetTicks()) {}
};
