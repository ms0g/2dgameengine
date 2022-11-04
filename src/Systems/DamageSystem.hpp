#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.hpp"
#include "../Events/CollisionEvent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/ProjectileComponent.hpp"
#include "../Components/HealthComponent.hpp"


class DamageSystem : public System {
public:
    DamageSystem() {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeToEvent<DamageSystem, CollisionEvent>(this, &DamageSystem::onCollision);
    }

    void onCollision(CollisionEvent& event) {
        Entity a = event.a;
        Entity b = event.b;
        Logger::Log("Damage system received an event collision between entities " + std::to_string(a.GetID()) +
                    " and " + std::to_string(b.GetID()));

        if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
            OnProjectileHitsPlayer(a, b);
        }

        if (a.HasTag("player") && b.BelongsToGroup("projectiles")) {
            OnProjectileHitsPlayer(b, a);
        }

        if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
            OnProjectileHitsEnemy(a, b);
        }

        if (a.BelongsToGroup("enemies") && b.BelongsToGroup("projectiles")) {
            OnProjectileHitsEnemy(b, a);
        }

    }

    void OnProjectileHitsPlayer(Entity projectile, Entity player) {
        const auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

        if (!projectileComponent.isFriendly) {
            auto& health = player.GetComponent<HealthComponent>();

            health.healthPercentage -= projectileComponent.hitPercentDamage;

            if (health.healthPercentage <= 0) {
                player.Kill();
            }

            projectile.Kill();
        }
    }

    void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
        const auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

        if (projectileComponent.isFriendly) {
            auto& health = enemy.GetComponent<HealthComponent>();

            health.healthPercentage -= projectileComponent.hitPercentDamage;

            if (health.healthPercentage <= 0) {
                enemy.Kill();
            }

            projectile.Kill();
        }
    }

    void Update() {

    }
};
