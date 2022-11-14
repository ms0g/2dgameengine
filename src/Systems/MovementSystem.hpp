#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"


class MovementSystem : public System {
public:
    MovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void Update(double deltaTime, int mapWidth, int mapHeight) {
        for (auto& entity: GetSystemEntities()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            const auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

            transform.position.x += rigidBody.velocity.x * deltaTime;
            transform.position.y += rigidBody.velocity.y * deltaTime;

            if ((transform.position.x < 0 || transform.position.x > mapWidth ||
                 transform.position.y < 0 || transform.position.y > mapHeight) && !entity.HasTag("player")) {
                entity.Kill();
            }
        }
    }
};
