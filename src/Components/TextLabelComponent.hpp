#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "glm/glm.hpp"

struct TextLabelComponent {
    glm::vec2 position{};
    std::string text;
    std::string assetID;
    SDL_Color color;
    bool isFixed;

    explicit TextLabelComponent(glm::vec2 position = glm::vec2{0},
                                std::string text = "",
                                std::string assetID = "",
                                const SDL_Color& color = {0},
                                bool isFixed = true) :
            position(position),
            text(std::move(text)),
            assetID(std::move(assetID)),
            color(color),
            isFixed(isFixed) {}
};
