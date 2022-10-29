#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.hpp"
#include "../Components/CameraFollowComponent.hpp"


class CameraMovementSystem : public System {
public:
    CameraMovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<CameraFollowComponent>();
    }

    void Update(SDL_Rect& camera, size_t windowWidth, size_t windowHeight, size_t mapWidth, size_t mapHeight) {
        for (const auto& entity: GetSystemEntities()) {
            const auto& transform = entity.GetComponent<TransformComponent>();

            if (transform.position.x + (camera.w / 2) < mapWidth)
                camera.x = transform.position.x - (windowWidth / 2);

            if (transform.position.y + (camera.h / 2) < mapHeight)
                camera.y = transform.position.y - (windowHeight / 2);

            // Keep camera rectangle view inside the screen limits
            camera.x = camera.x < 0 ? 0 : camera.x;
            camera.y = camera.y < 0 ? 0 : camera.y;
            camera.x = (camera.x + camera.w > mapWidth) ? mapWidth - camera.w : camera.x;
            camera.y = (camera.y + camera.h > mapHeight) ? mapHeight - camera.h : camera.y;
        }
    }
};
