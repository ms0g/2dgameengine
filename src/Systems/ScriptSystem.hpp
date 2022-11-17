#pragma once

#include "../ECS/ECS.h"
#include "../Components/ScriptComponent.hpp"

void SetEntityPosition(Entity entity, double x, double y) {
    if (entity.HasComponent<TransformComponent>()) {
        auto& transform = entity.GetComponent<TransformComponent>();
        transform.position.x = x;
        transform.position.y = y;
    } else {
        Logger::Error("Trying to set the position of an entity that has no transform component.");
    }
}

class ScriptSystem : public System {
public:
    ScriptSystem() {
        RequireComponent<ScriptComponent>();
    }

    void CreateLuaBindings(sol::state& lua) {
        // Create the "entity" usertype so Lua knows what an entity is
        lua.new_usertype<Entity>(
                "entity",
                "get_id", &Entity::GetID,
                "destroy", &Entity::Kill,
                "has_tag", &Entity::HasTag,
                "belongs_to_group", &Entity::BelongsToGroup
        );

        // Create all the bindings between C++ and Lua functions
        lua.set_function("set_position", SetEntityPosition);
    }

    void Update(double deltaTime, int ellapsedTime) {
        for (const auto& entity: GetSystemEntities()) {
            const auto script = entity.GetComponent<ScriptComponent>();
            script.func(entity, deltaTime, ellapsedTime);

        }
    }
};
