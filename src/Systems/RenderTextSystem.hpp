#pragma once

#include <SDL2/SDL_ttf.h>
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include "../Components/TextLabelComponent.hpp"


class RenderTextSystem : public System {
public:
    RenderTextSystem() {
        RequireComponent<TextLabelComponent>();
    }

    void Update(SDL_Renderer* renderer, const SDL_Rect& camera, const std::unique_ptr<AssetManager>& assetManager) {
        for (const auto& entity: GetSystemEntities()) {
            const auto& textlabel = entity.GetComponent<TextLabelComponent>();

            SDL_Surface* surface = TTF_RenderText_Blended(
                    assetManager->GetFont(textlabel.assetID),
                    textlabel.text.c_str(),
                    textlabel.color);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int labelWidth = 0;
            int labelHeight = 0;

            SDL_QueryTexture(texture, nullptr, nullptr, &labelWidth, &labelHeight);
            SDL_Rect dstRect = {
                    static_cast<int>(textlabel.position.x - (textlabel.isFixed ? 0 : camera.x)),
                    static_cast<int>(textlabel.position.y - (textlabel.isFixed ? 0 : camera.y)),
                    labelWidth,
                    labelHeight
            };

            SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
            SDL_DestroyTexture(texture);

        }
    }
};
