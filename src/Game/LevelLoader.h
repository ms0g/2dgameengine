#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "sol/sol.hpp"

class AssetManager;
class Registry;
class LevelLoader {
public:
    LevelLoader();

    ~LevelLoader();

    void LoadLevel(sol::state& lua,
                   int windowWidth,
                   int& mapWidth,
                   int& mapHeight,
                   SDL_Renderer* renderer,
                   const std::unique_ptr<AssetManager>& assetManager,
                   const std::unique_ptr<Registry>& registry);
};


