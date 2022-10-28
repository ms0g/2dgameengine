#pragma once

#include <string>
#include <SDL2/SDL.h>

struct SpriteComponent {
    std::string assetID;
    SDL_Rect srcRect{};
    int width;
    int height;
    int zIndex;

    explicit SpriteComponent(std::string assetID = "",
                             int width = 0,
                             int height = 0,
                             int zIndex = 0,
                             int srcRectX = 0,
                             int srcRectY = 0) {
        this->width = width;
        this->height = height;
        this->zIndex = zIndex;
        this->assetID = std::move(assetID);
        this->srcRect = {srcRectX, srcRectY, width, height};
    }
};
