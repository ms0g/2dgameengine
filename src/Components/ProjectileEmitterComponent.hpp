#pragma once

#include "glm/glm.hpp"

struct ProjectileEmitterComponent {
    glm::vec2 projectileVelocity{};
    int repeatFrequency;
    int projectileDuration;
    int hitPercentDamage;
    bool isFriendly;
    int lastEmissionTime;

    explicit ProjectileEmitterComponent(
            glm::vec2 projectileVelocity = glm::vec2{0},
            int repeatFrequency = 0,
            int projectileDuration = 10000,
            int hitPercentDamage = 10,
            bool isFriendly = false,
            int lastEmissionTime = 0) {
        this->projectileVelocity = projectileVelocity;
        this->repeatFrequency = repeatFrequency;
        this->projectileDuration = projectileDuration;
        this->hitPercentDamage = hitPercentDamage;
        this->isFriendly = isFriendly;
        this->lastEmissionTime = lastEmissionTime;

    }
};