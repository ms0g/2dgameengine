#pragma once

#include "../ECS/ECS.h"
#include "../Components/ScriptComponent.hpp"

class ScriptSystem : public System {
public:
    ScriptSystem() {
        RequireComponent<ScriptComponent>();
    }

    void Update() {
        for (const auto& entity: GetSystemEntities()) {
            const auto script = entity.GetComponent<ScriptComponent>();
            script.func();

        }
    }
};
