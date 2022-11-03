#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include "../Components/TransformComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/HealthComponent.hpp"


class RenderHealthBarSystem : public System {
public:
    RenderHealthBarSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
        RequireComponent<HealthComponent>();
    }

    void Update(SDL_Renderer* renderer, const SDL_Rect& camera, const std::unique_ptr<AssetManager>& assetManager) {
        for (auto entity: GetSystemEntities()) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();
            const auto health = entity.GetComponent<HealthComponent>();

            // Draw a the health bar with the correct color for the percentage
            SDL_Color healthBarColor = {255, 255, 255};

            if (health.healthPercentage >= 0 && health.healthPercentage < 40) {
                // 0-40 = red
                healthBarColor = {255, 0, 0};
            }
            if (health.healthPercentage >= 40 && health.healthPercentage < 80) {
                // 40-80 = yellow
                healthBarColor = {255, 255, 0};
            }
            if (health.healthPercentage >= 80 && health.healthPercentage <= 100) {
                // 80-100 = green
                healthBarColor = {0, 255, 0};
            }

            // Position the health bar indicator in the top-right part of the entity sprite
            int healthBarWidth = 15;
            int healthBarHeight = 3;
            double healthBarPosX = (transform.position.x + (sprite.width / 4) * transform.scale.x) - camera.x;
            double healthBarPosY = (transform.position.y - (sprite.height - 22) * transform.scale.y) - camera.y;

            SDL_Rect healthBarRectangle = {
                    static_cast<int>(healthBarPosX),
                    static_cast<int>(healthBarPosY),
                    static_cast<int>(healthBarWidth * (health.healthPercentage / 100.0)),
                    static_cast<int>(healthBarHeight)
            };
            SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, 255);
            SDL_RenderFillRect(renderer, &healthBarRectangle);

            // Render the health percentage text label indicator
            std::string healthText = std::to_string(health.healthPercentage);
            SDL_Surface* surface = TTF_RenderText_Blended(assetManager->GetFont("pico8-font-5"), healthText.c_str(),
                                                          healthBarColor);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int labelWidth = 0;
            int labelHeight = 0;
            SDL_QueryTexture(texture, nullptr, nullptr, &labelWidth, &labelHeight);
            SDL_Rect healthBarTextRectangle = {
                    static_cast<int>(healthBarPosX),
                    static_cast<int>(healthBarPosY) + 5,
                    labelWidth,
                    labelHeight
            };

            SDL_RenderCopy(renderer, texture, nullptr, &healthBarTextRectangle);

            SDL_DestroyTexture(texture);

        }

    }
};


