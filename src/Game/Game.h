#pragma once

#include <cstdlib>
#include <memory>
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

    void LoadLevel(int level);

    SDL_Window* window{};
    SDL_Renderer* renderer{};
    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<EventBus> eventBus;
    bool isRunning;
    size_t windowHeight{};
    size_t windowWidth{};
    uint32_t millisecsPreviousFrame{0};
    static constexpr int FPS = 60;
    static constexpr int MILLISECS_PER_FRAME = 1000 / FPS;

};
