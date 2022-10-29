#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.hpp"


class ProjectileLifeCycleSystem : public System {
public:
    ProjectileLifeCycleSystem() {
        RequireComponent<ProjectileComponent>();
    }

    void Update() {
        for (auto& entity: GetSystemEntities()) {
            auto& projectile = entity.GetComponent<ProjectileComponent>();

            if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
                entity.Kill();
            }
        }
    }
};
