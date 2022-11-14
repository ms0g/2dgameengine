#pragma once

#include <string>
#include <utility>
#include <SDL2/SDL.h>

struct SpriteComponent {
    std::string assetID;
    SDL_Rect srcRect{};
    int width;
    int height;
    int zIndex;
    SDL_RendererFlip flip;
    bool isFixed;

    explicit SpriteComponent(std::string assetID = "",
                             int width = 0,
                             int height = 0,
                             int zIndex = 0,
                             bool isFixed = false,
                             int srcRectX = 0,
                             int srcRectY = 0) :
            assetID(std::move(assetID)),
            width(width),
            height(height),
            zIndex(zIndex),
            flip(SDL_FLIP_NONE),
            isFixed(isFixed),
            srcRect({srcRectX, srcRectY, width, height}) {}
};
