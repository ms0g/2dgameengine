#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/ProjectileEmitterComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/TransformComponent.hpp"


class ProjectileEmitSystem : public System {
public:
    ProjectileEmitSystem() {
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(std::unique_ptr<Registry>& registry) {
        for (const auto& entity: GetSystemEntities()) {
            const auto& transform = entity.GetComponent<TransformComponent>();
            auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

            if (SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency) {
                glm::vec2 projectilePosition = transform.position;
                if (entity.HasComponent<SpriteComponent>()) {
                    const auto sprite = entity.GetComponent<SpriteComponent>();
                    projectilePosition.x += (transform.scale.x * sprite.width / 2);
                    projectilePosition.y += (transform.scale.y * sprite.height / 2);
                }

                // Add a new projectile entity to registry
                auto projectile = registry->CreateEntity();
                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2{1.0, 1.0}, 0.0);
                projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
                projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 5);
                projectile.AddComponent<BoxColliderComponent>(4, 4);

                projectileEmitter.lastEmissionTime = SDL_GetTicks();

            }


        }
    }
};