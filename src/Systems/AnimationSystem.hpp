#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/AnimationComponent.hpp"
#include "../Components/SpriteComponent.hpp"


class AnimationSystem : public System {
public:
    AnimationSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<AnimationComponent>();
    }

    void Update() {
        for (const auto& entity: GetSystemEntities()) {
            auto& animation = entity.GetComponent<AnimationComponent>();
            auto& sprite = entity.GetComponent<SpriteComponent>();

            animation.currentFrame =
                    ((SDL_GetTicks() - animation.startTime) * animation.frameSpeedRate / 1000) % animation.numFrames;
            sprite.srcRect.x = animation.currentFrame * sprite.width;
        }
    }
};
