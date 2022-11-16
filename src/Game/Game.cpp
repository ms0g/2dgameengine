#include "Game.h"
#include <SDL2/SDL.h>
#include "imgui/imgui_impl_sdl.h"
#include "LevelLoader.h"
#include "../Systems/MovementSystem.hpp"
#include "../Systems/AnimationSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/CollisionSystem.hpp"
#include "../Systems/DamageSystem.hpp"
#include "../Systems/KeyboardControlSystem.hpp"
#include "../Systems/CameraMovementSystem.hpp"
#include "../Systems/ProjectileEmitSystem.hpp"
#include "../Systems/ProjectileLifeCycleSystem.hpp"
#include "../Systems/RenderTextSystem.hpp"
#include "../Systems/RenderHealthBarSystem.hpp"

#ifdef ENABLE_DEBUG

#include "../Systems/RenderColliderSystem.hpp"
#include "../Systems/RenderGUISystem.hpp"

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

    if (TTF_Init() != 0) {
        Logger::Error("Error initializing SDL TTF");
        return;
    }

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
#ifdef ENABLE_DEBUG
    // Initialize the ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);
#endif
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
#ifdef ENABLE_DEBUG
        ImGui_ImplSDL2_ProcessEvent(&event);
        ImGuiIO& io = ImGui::GetIO();

        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
        io.MousePos = ImVec2(mouseX, mouseY);
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
#endif
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

void Game::Setup() {
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<CameraMovementSystem>();
    registry->AddSystem<ProjectileEmitSystem>();
    registry->AddSystem<ProjectileLifeCycleSystem>();
    registry->AddSystem<RenderTextSystem>();
    registry->AddSystem<RenderHealthBarSystem>();

#ifdef ENABLE_DEBUG
    registry->AddSystem<RenderColliderSystem>();
    registry->AddSystem<RenderGUISystem>();
#endif
    LevelLoader loader;
    lua.open_libraries(sol::lib::base);
    loader.LoadLevel(lua, windowWidth, mapWidth, mapHeight, renderer, assetManager, registry);
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
    registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);

    // Update the registry to process the entities that are waiting to be created/deleted
    registry->Update();

    // Invoke all systems to update
    registry->GetSystem<MovementSystem>().Update(deltaTime, mapWidth, mapHeight);
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    registry->GetSystem<ProjectileEmitSystem>().Update(registry);
    registry->GetSystem<ProjectileLifeCycleSystem>().Update();
    registry->GetSystem<CameraMovementSystem>().Update(camera, windowWidth, windowHeight, mapWidth, mapHeight);
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all systems to render
    registry->GetSystem<RenderSystem>().Update(renderer, camera, assetManager);
    registry->GetSystem<RenderTextSystem>().Update(renderer, camera, assetManager);
    registry->GetSystem<RenderHealthBarSystem>().Update(renderer, camera, assetManager);

#ifdef ENABLE_DEBUG
    registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);
    registry->GetSystem<RenderGUISystem>().Update(registry, camera);
#endif
    SDL_RenderPresent(renderer);

}

void Game::Destroy() {
#ifdef ENABLE_DEBUG
    // Clean up ImGui
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif
    // Clean up SDL
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}




