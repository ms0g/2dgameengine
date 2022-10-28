#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.hpp"
#include "../Events/KeyPressedEvent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/KeyboardControlledComponent.hpp"
#include "../Components/SpriteComponent.hpp"

class KeyboardControlSystem : public System {
public:
    KeyboardControlSystem() {
        RequireComponent<KeyboardControlledComponent>();
        RequireComponent<SpriteComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<KeyboardControlSystem, KeyPressedEvent>(this, &KeyboardControlSystem::onKeyPressed);
    }

    void onKeyPressed(KeyPressedEvent& event) {
        for (auto& entity: GetSystemEntities()) {
            const auto& keyboardControl = entity.GetComponent<KeyboardControlledComponent>();
            auto& sprite = entity.GetComponent<SpriteComponent>();
            auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

            switch (event.symbol) {
                case SDLK_UP:
                    rigidBody.velocity = keyboardControl.upVelocity;
                    sprite.srcRect.y = sprite.height * 0;
                    break;
                case SDLK_RIGHT:
                    rigidBody.velocity = keyboardControl.rightVelocity;
                    sprite.srcRect.y = sprite.height * 1;
                    break;
                case SDLK_DOWN:
                    rigidBody.velocity = keyboardControl.downVelocity;
                    sprite.srcRect.y = sprite.height * 2;
                    break;
                case SDLK_LEFT:
                    rigidBody.velocity = keyboardControl.leftVelocity;
                    sprite.srcRect.y = sprite.height * 3;
                    break;
            }

        }
    }

    void Update() {

    }
};
