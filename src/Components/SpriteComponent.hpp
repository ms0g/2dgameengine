#pragma once

#include <string>
#include <SDL2/SDL.h>

struct SpriteComponent {
    std::string assetID;
    SDL_Rect srcRect{};
    int width;
    int height;
    int zIndex;
    bool isFixed;

    explicit SpriteComponent(std::string assetID = "",
                             int width = 0,
                             int height = 0,
                             int zIndex = 0,
                             bool isFixed = false,
                             int srcRectX = 0,
                             int srcRectY = 0) {
        this->assetID = std::move(assetID);
        this->width = width;
        this->height = height;
        this->zIndex = zIndex;
        this->isFixed = isFixed;
        this->srcRect = {srcRectX, srcRectY, width, height};
    }
};
