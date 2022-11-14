#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include "../Components/TransformComponent.hpp"
#include "../Components/SpriteComponent.hpp"



class RenderSystem : public System {
public:
    RenderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer* renderer, const SDL_Rect& camera, const std::unique_ptr<AssetManager>& assetManager) {
        // sort
        auto renderableEntities = GetSystemEntities();
        std::sort(renderableEntities.begin(), renderableEntities.end(), [](const Entity& e1, const Entity& e2) {
            return e1.GetComponent<SpriteComponent>().zIndex < e2.GetComponent<SpriteComponent>().zIndex;
        });

        for (const auto& entity: renderableEntities) {
            const auto& transform = entity.GetComponent<TransformComponent>();
            const auto& sprite = entity.GetComponent<SpriteComponent>();

            SDL_Rect srcRect = sprite.srcRect;

            SDL_Rect dstRect = {
                    static_cast<int>(transform.position.x - (sprite.isFixed ? 0 : camera.x)),
                    static_cast<int>(transform.position.y - (sprite.isFixed ? 0 : camera.y)),
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y)
            };
            SDL_RenderCopyEx(
                    renderer,
                    assetManager->GetTexture(sprite.assetID),
                    &srcRect,
                    &dstRect,
                    transform.rotation,
                    nullptr,
                    sprite.flip);
        }
    }
};

