#pragma once

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Events/CollisionEvent.hpp"
#include "../EventBus/EventBus.hpp"

class CollisionSystem : public System {
public:
    CollisionSystem() {
        RequireComponent<BoxColliderComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(std::unique_ptr<EventBus>& eventBus) {
        auto entities = GetSystemEntities();
        for (auto aIt = entities.begin(); aIt != entities.end(); aIt++) {
            auto aCollider = aIt->GetComponent<BoxColliderComponent>();
            auto aTransform = aIt->GetComponent<TransformComponent>();

            for (auto bIt = aIt + 1; bIt != entities.end(); bIt++) {
                auto bCollider = bIt->GetComponent<BoxColliderComponent>();
                auto bTransform = bIt->GetComponent<TransformComponent>();

                if (CheckAABBCollision(
                        aTransform.position.x + aCollider.offset.x,
                        aTransform.position.y + aCollider.offset.y,
                        aCollider.width,
                        aCollider.height,
                        bTransform.position.x + bCollider.offset.x,
                        bTransform.position.y + bCollider.offset.y,
                        bCollider.width,
                        bCollider.height)) {

                    Logger::Log("Entity " + std::to_string(aIt->GetID()) + " has been collided to entity=" +
                                std::to_string(bIt->GetID()));

                    eventBus->EmitEvent<CollisionEvent>(*aIt, *bIt);
                }
            }
        }
    }

    bool CheckAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
        return (
                aX < bX + bW &&
                aX + aW > bX &&
                aY < bY + bH &&
                aY + aH > bY);
    }
};
