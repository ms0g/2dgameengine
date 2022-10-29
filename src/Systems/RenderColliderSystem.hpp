#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"

class RenderColliderSystem : public System {
public:
    RenderColliderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void Update(SDL_Renderer* renderer, SDL_Rect& camera) {
        for (const auto& entity: GetSystemEntities()) {
            const auto& transform = entity.GetComponent<TransformComponent>();
            const auto& collider = entity.GetComponent<BoxColliderComponent>();

            SDL_Rect colliderRect{
                    static_cast<int>(transform.position.x + collider.offset.x - camera.x),
                    static_cast<int>(transform.position.y + collider.offset.y - camera.y),
                    collider.width,
                    collider.height
            };

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &colliderRect);
        }
    }
};
