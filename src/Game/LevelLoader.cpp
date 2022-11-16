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

    /**
     * Read the level assets
     */
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

    /**
     *  Read the level tilemap information
     */
    sol::table map = level["tilemap"];
    std::string mapFilePath = map["map_file"];
    std::string mapTextureAssetId = map["texture_asset_id"];
    int mapNumRows = map["num_rows"];
    int mapNumCols = map["num_cols"];
    int tileSize = map["tile_size"];
    double mapScale = map["scale"];
    std::fstream mapFile;
    mapFile.open(mapFilePath);
    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(x * (mapScale * tileSize), y * (mapScale * tileSize)),
                                                  glm::vec2(mapScale, mapScale), 0.0);
            tile.AddComponent<SpriteComponent>(mapTextureAssetId, tileSize, tileSize, 0, false, srcRectX, srcRectY);
        }
    }
    mapFile.close();

    mapWidth = mapNumCols * tileSize * mapScale;
    mapHeight = mapNumRows * tileSize * mapScale;

    /**
     * Read the level entities and their components
     */
    sol::table entities = level["entities"];

    for (int i = 0;; ++i) {
        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt) {
            break;
        }

        sol::table entity = entities[i];

        Entity newEntity = registry->CreateEntity();

        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        // Group
        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        // Components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            // Transform
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt) {
                newEntity.AddComponent<TransformComponent>(
                        glm::vec2(
                                entity["components"]["transform"]["position"]["x"],
                                entity["components"]["transform"]["position"]["y"]
                        ),
                        glm::vec2(
                                entity["components"]["transform"]["scale"]["x"].get_or(1.0),
                                entity["components"]["transform"]["scale"]["y"].get_or(1.0)
                        ),
                        entity["components"]["transform"]["rotation"].get_or(0.0)
                );
            }

            // RigidBody
            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<RigidBodyComponent>(
                        glm::vec2(
                                entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                                entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                        )
                );
            }

            // Sprite
            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt) {
                newEntity.AddComponent<SpriteComponent>(
                        entity["components"]["sprite"]["texture_asset_id"],
                        entity["components"]["sprite"]["width"],
                        entity["components"]["sprite"]["height"],
                        entity["components"]["sprite"]["z_index"].get_or(1),
                        entity["components"]["sprite"]["fixed"].get_or(false),
                        entity["components"]["sprite"]["src_rect_x"].get_or(0),
                        entity["components"]["sprite"]["src_rect_y"].get_or(0)
                );
            }

            // Animation
            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<AnimationComponent>(
                        entity["components"]["animation"]["num_frames"].get_or(1),
                        entity["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

            // BoxCollider
            sol::optional<sol::table> collider = entity["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                newEntity.AddComponent<BoxColliderComponent>(
                        entity["components"]["boxcollider"]["width"],
                        entity["components"]["boxcollider"]["height"],
                        glm::vec2(
                                entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
                                entity["components"]["boxcollider"]["offset"]["y"].get_or(0)
                        )
                );
            }

            // Health
            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<HealthComponent>(
                        static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                );
            }

            // ProjectileEmitter
            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                newEntity.AddComponent<ProjectileEmitterComponent>(
                        glm::vec2(
                                entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                                entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                        ),
                        static_cast<int>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) *
                        1000,
                        static_cast<int>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) *
                        1000,
                        static_cast<int>(entity["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(
                                10)),
                        entity["components"]["projectile_emitter"]["friendly"].get_or(false)
                );
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<CameraFollowComponent>();
            }

            // KeyboardControlled
            sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                newEntity.AddComponent<KeyboardControlledComponent>(
                        glm::vec2(
                                entity["components"]["keyboard_controller"]["up_velocity"]["x"],
                                entity["components"]["keyboard_controller"]["up_velocity"]["y"]
                        ),
                        glm::vec2(
                                entity["components"]["keyboard_controller"]["right_velocity"]["x"],
                                entity["components"]["keyboard_controller"]["right_velocity"]["y"]
                        ),
                        glm::vec2(
                                entity["components"]["keyboard_controller"]["down_velocity"]["x"],
                                entity["components"]["keyboard_controller"]["down_velocity"]["y"]
                        ),
                        glm::vec2(
                                entity["components"]["keyboard_controller"]["left_velocity"]["x"],
                                entity["components"]["keyboard_controller"]["left_velocity"]["y"]
                        )
                );
            }
        }
    }
}
