#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.hpp"
#include "../Events/KeyPressedEvent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"


class KeyboardControlSystem : public System {
public:
    KeyboardControlSystem() {
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<KeyboardControlSystem, KeyPressedEvent>(this, &KeyboardControlSystem::onKeyPressed);
    }

    void onKeyPressed(KeyPressedEvent& event) {
        Logger::Log("Key pressed even emitted [" + std::to_string(event.symbol) + "] " + static_cast<char>(event.symbol));
    }

    void Update() {

    }
};
