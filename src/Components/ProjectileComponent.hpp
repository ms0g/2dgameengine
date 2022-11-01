#pragma once

#include <SDL2/SDL.h>

struct ProjectileComponent {
    int duration;
    int hitPercentDamage;
    bool isFriendly;
    uint32_t startTime;

    explicit ProjectileComponent(
            int duration = 10000,
            int hitPercentDamage = 0,
            bool isFriendly = false) {
        this->duration = duration;
        this->hitPercentDamage = hitPercentDamage;
        this->isFriendly = isFriendly;
        this->startTime = SDL_GetTicks();
    }
};