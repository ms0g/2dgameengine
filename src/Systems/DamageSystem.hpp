#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.hpp"
#include "../Events/CollisionEvent.hpp"
#include "../Components/BoxColliderComponent.hpp"


class DamageSystem : public System {
public:
    DamageSystem() {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<DamageSystem, CollisionEvent>(this, &DamageSystem::onCollision);
    }

    void onCollision(CollisionEvent& event) {
        Logger::Log("Damage system received an event collision between entities " + std::to_string(event.a.GetID()) +
                    " and " + std::to_string(event.b.GetID()));
        event.a.Kill();
        event.b.Kill();
    }

    void Update() {

    }
};
