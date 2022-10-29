#include "Game.h"
#include <SDL2/SDL.h>
#include <fstream>
#include "glm/glm.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/AnimationComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/KeyboardControlledComponent.hpp"
#include "../Components/CameraFollowComponent.hpp"
#include "../Systems/MovementSystem.hpp"
#include "../Systems/AnimationSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/CollisionSystem.hpp"
#include "../Systems/DamageSystem.hpp"
#include "../Systems/KeyboardControlSystem.hpp"
#include "../Systems/CameraMovementSystem.hpp"

#ifdef ENABLE_COLLIDER_DEBUG
#include "../Systems/RenderColliderSystem.hpp"
#endif

Game::Game() :
        isRunning(false),
        registry(std::make_unique<Registry>()),
        assetManager(std::make_unique<AssetManager>()),
        eventBus(std::make_unique<EventBus>()) {
    Logger::Log("Game ctor called");
}

Game::~Game() {
    Logger::Log("Game dtor called");
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Error("Error initializing SDL");
        return;
    }
    windowWidth = 800;
    windowHeight = 600;
    window = SDL_CreateWindow(
            nullptr,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
            SDL_WINDOW_BORDERLESS);

    if (!window) {
        Logger::Error("Error creating SDL Window");
        return;
    }

    // Initialize the camera view with the entire screen area
    camera.x = 0;
    camera.y = 0;
    camera.w = windowWidth;
    camera.h = windowHeight;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        Logger::Error("Error creating SDL Renderer");
        return;
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_RESIZABLE);
    isRunning = true;
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    isRunning = false;

                eventBus->EmitEvent<KeyPressedEvent>(event.key.keysym.sym);
                break;
        }
    }

}

void Game::LoadLevel(int level) {
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<CameraMovementSystem>();

#ifdef ENABLE_COLLIDER_DEBUG
    registry->AddSystem<RenderColliderSystem>();
#endif

    // Adding assets to Asset Store
    assetManager->AddTexture(renderer, "tank-image", "../assets/images/tank-panther-right.png");
    assetManager->AddTexture(renderer, "truck-image", "../assets/images/truck-ford-right.png");
    assetManager->AddTexture(renderer, "chopper-image", "../assets/images/chopper-spritesheet.png");
    assetManager->AddTexture(renderer, "radar-image", "../assets/images/radar.png");
    assetManager->AddTexture(renderer, "tilemap-image", "../assets/tilemaps/jungle.png");

    // Load the tilemap
    int tileSize = 32;
    double tileScale = 2.0;
    int mapNumCols = 25;
    int mapNumRows = 20;

    std::ifstream mapFile;
    if (mapFile.open("../assets/tilemaps/jungle.map");!mapFile.is_open())
        Logger::Error("Error on reading Map file!");

    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(
                    glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
                    glm::vec2(tileScale, tileScale), 0.0);
            tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY);
        }
    }
    mapFile.close();
    mapWidth = mapNumCols * tileSize * tileScale;
    mapHeight = mapNumRows * tileSize * tileScale;

    // Create entities
    auto chopper = registry->CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2{100.0, 100.0}, glm::vec2{1.0, 1.0}, 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2{0.0, 0.0});
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    chopper.AddComponent<KeyboardControlledComponent>(glm::vec2{0, -80}, glm::vec2{80, 0},
                                                      glm::vec2{0, 80}, glm::vec2{-80, 0});
    chopper.AddComponent<CameraFollowComponent>();

    auto radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2{windowWidth - 74, 10.0}, glm::vec2{1.0, 1.0}, 0.0);
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    auto truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2{10.0, 10.0}, glm::vec2{1.0, 1.0}, 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2{20.0, 0});
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 3);
    truck.AddComponent<BoxColliderComponent>(32, 32);

    auto tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2{500.0, 10.0}, glm::vec2{1.0, 1.0}, 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2{-30.0, 0});
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 4);
    tank.AddComponent<BoxColliderComponent>(32, 32);

}


void Game::Setup() {
    LoadLevel(1);
}

void Game::Update() {
    auto timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
        SDL_Delay(timeToWait);

    // difference in ticks since the last frame, in seconds
    auto deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    // Store the previous frame time
    millisecsPreviousFrame = SDL_GetTicks();

    // Reset all event handlers for the current frame
    eventBus->Reset();

    // Perform the subscription of events for all systems
    registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);

    // Update the registry to process the entities that are waiting to be created/deleted
    registry->Update();

    // Invoke all systems to update
    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    registry->GetSystem<CameraMovementSystem>().Update(camera, windowWidth, windowHeight, mapWidth, mapHeight);
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all systems to render
    registry->GetSystem<RenderSystem>().Update(renderer, camera, assetManager);
#ifdef ENABLE_COLLIDER_DEBUG
    registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);
#endif

    SDL_RenderPresent(renderer);

}

void Game::Destroy() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}




