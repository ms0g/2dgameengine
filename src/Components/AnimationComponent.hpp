#pragma once

#include <SDL2/SDL.h>

struct AnimationComponent {
    int numFrames;
    int currentFrame;
    int frameSpeedRate;
    bool shouldLoop;
    uint32_t startTime;

    explicit AnimationComponent(int numFrames = 1,
                                int frameSpeedRate = 1,
                                bool shouldLoop = true) {
        this->numFrames = numFrames;
        this->currentFrame = 1;
        this->frameSpeedRate = frameSpeedRate;
        this->shouldLoop = shouldLoop;
        this->startTime = SDL_GetTicks();
    }
};
