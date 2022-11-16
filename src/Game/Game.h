#pragma once

#include <cstdlib>
#include <memory>
#include "sol/sol.hpp"
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include "../EventBus/EventBus.hpp"

class Game {
public:
    Game();

    ~Game();

    void Initialize();

    void Run();

    void Destroy();

private:
    void ProcessInput();

    void Update();

    void Render();

    void Setup();

    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Rect camera{};
    sol::state lua;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<EventBus> eventBus;
    bool isRunning;
    int windowWidth{800};
    int windowHeight{600};
    int mapWidth{};
    int mapHeight{};

    uint32_t millisecsPreviousFrame{0};
    static constexpr int FPS = 60;
    static constexpr int MILLISECS_PER_FRAME = 1000 / FPS;

};
