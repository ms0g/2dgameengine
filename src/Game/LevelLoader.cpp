#include "LevelLoader.h"
#include <fstream>
#include "sol/sol.hpp"
#include "../Logger/Logger.h"
#include "../AssetManager/AssetManager.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/AnimationComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/KeyboardControlledComponent.hpp"
#include "../Components/CameraFollowComponent.hpp"
#include "../Components/ProjectileEmitterComponent.hpp"
#include "../Components/HealthComponent.hpp"
#include "../Components/TextLabelComponent.hpp"


LevelLoader::LevelLoader() {
    Logger::Log("Level Loader ctor called");
}

LevelLoader::~LevelLoader() {
    Logger::Log("Level Loader dtor called");
}

void LevelLoader::LoadLevel(sol::state& lua,
                            int windowWidth,
                            int& mapWidth,
                            int& mapHeight,
                            SDL_Renderer* renderer,
                            const std::unique_ptr<AssetManager>& assetManager,
                            const std::unique_ptr<Registry>& registry) {
    // This checks the syntax of our script, but it does not execute the script
    sol::load_result script = lua.load_file("../assets/scripts/Level" + std::to_string(1) + ".lua");

    if (!script.valid()) {
        sol::error err = script;
        Logger::Error("Error loading lua script " + std::string{err.what()});
        return;
    }

    // Executes the script
    lua.script_file("../assets/scripts/Level" + std::to_string(1) + ".lua");

    // Read the big table for the current level
    sol::table level = lua["Level"];

    // Read the level assets
    sol::table assets = level["assets"];

    for (int i = 0;; ++i) {
        sol::optional<sol::table> hasAsset = assets[i];
        if (hasAsset == sol::nullopt) {
            break;
        }
        sol::table asset = assets[i];
        std::string assetType = asset["type"];
        std::string assetId = asset["id"];
        if (assetType == "texture") {
            assetManager->AddTexture(renderer, assetId, asset["file"]);
            Logger::Log("A new texture asset was added to the asset store, id: " + assetId);
        }
        if (assetType == "font") {
            assetManager->AddFont(assetId, asset["file"], asset["font_size"]);
            Logger::Log("A new font asset was added to the asset store, id: " + assetId);
        }
    }

//    // Adding assets to Asset Store
//    assetManager->AddTexture(renderer, "tank-texture", "../assets/textures/tank-panther-right.png");
//    assetManager->AddTexture(renderer, "truck-texture", "../assets/textures/truck-ford-right.png");
//    assetManager->AddTexture(renderer, "chopper-texture", "../assets/textures/chopper-spritesheet.png");
//    assetManager->AddTexture(renderer, "tree-texture", "../assets/textures/tree.png");
//    assetManager->AddTexture(renderer, "radar-texture", "../assets/textures/radar.png");
//    assetManager->AddTexture(renderer, "tilemap-texture", "../assets/tilemaps/jungle.png");
//    assetManager->AddTexture(renderer, "bullet-texture", "../assets/textures/bullet.png");
//    assetManager->AddFont("charriot-font", "../assets/fonts/charriot.ttf", 20);
//    assetManager->AddFont("pico8-font-5", "../assets/fonts/pico8.ttf", 5);
//    assetManager->AddFont("pico8-font-10", "../assets/fonts/pico8.ttf", 10);

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
            tile.Group("tiles");
            tile.AddComponent<TransformComponent>(
                    glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
                    glm::vec2(tileScale, tileScale), 0.0);
            tile.AddComponent<SpriteComponent>("tilemap-texture", tileSize, tileSize, 0, false, srcRectX, srcRectY);
        }
    }
    mapFile.close();
    mapWidth = mapNumCols * tileSize * tileScale;
    mapHeight = mapNumRows * tileSize * tileScale;

    // Create entities
    auto chopper = registry->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2{100.0, 100.0}, glm::vec2{1.0, 1.0}, 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2{0.0, 0.0});
    chopper.AddComponent<SpriteComponent>("chopper-texture", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<KeyboardControlledComponent>(glm::vec2{0, -80}, glm::vec2{80, 0},
                                                      glm::vec2{0, 80}, glm::vec2{-80, 0});
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2{150.0, 150.0}, 0, 10000, 10, true);
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);

    auto radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2{windowWidth - 74, 10.0}, glm::vec2{1.0, 1.0}, 0.0);
    radar.AddComponent<SpriteComponent>("radar-texture", 64, 64, 2, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    auto truck = registry->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2{10.0, 500.0}, glm::vec2{1.0, 1.0}, 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2{0.0, 0});
    truck.AddComponent<SpriteComponent>("truck-texture", 32, 32, 3);
    truck.AddComponent<BoxColliderComponent>(32, 32);
    truck.AddComponent<HealthComponent>(100);

    auto tank = registry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2{500.0, 500.0}, glm::vec2{1.0, 1.0}, 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2{20.0, 0});
    tank.AddComponent<SpriteComponent>("tank-texture", 32, 32, 4);
    tank.AddComponent<BoxColliderComponent>(32, 32);
    tank.AddComponent<HealthComponent>(100);

    auto treeA = registry->CreateEntity();
    treeA.Group("obstacles");
    treeA.AddComponent<TransformComponent>(glm::vec2{600.0, 495.0}, glm::vec2{1.0, 1.0}, 0.0);
    treeA.AddComponent<SpriteComponent>("tree-texture", 16, 32, 2);
    treeA.AddComponent<BoxColliderComponent>(16, 32);

    auto treeB = registry->CreateEntity();
    treeB.Group("obstacles");
    treeB.AddComponent<TransformComponent>(glm::vec2{400.0, 495.0}, glm::vec2{1.0, 1.0}, 0.0);
    treeB.AddComponent<SpriteComponent>("tree-texture", 16, 32, 2);
    treeB.AddComponent<BoxColliderComponent>(16, 32);

//    auto label = registry->CreateEntity();
//    SDL_Color white = {0, 255, 0};
//    label.AddComponent<TextLabelComponent>(glm::vec2{windowWidth / 2 - 40, 10},
//                                           "Chopper 1.0", "charriot-font", white, true);


}
