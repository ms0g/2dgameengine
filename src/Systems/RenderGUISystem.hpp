#pragma once

#include <SDL2/SDL.h>
#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/HealthComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"

class RenderGUISystem : public System {
public:
    RenderGUISystem() = default;

    void Update(const std::unique_ptr<Registry>& registry, const SDL_Rect& camera) {
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Show demo window
        if (ImGui::Begin("Spawn enemies")) {
            static int posX = 0, posY = 0;
            static int scaleX = 1, scaleY = 1;
            static int velX = 0, velY = 0;
            static int health = 100;
            static float rotation = 0.0, projAngle = 0.0, projSpeed = 100.0;
            static int projRepeat = 10, projDuration = 10;
            const char* sprites[] = {"tank-image", "truck-image"};
            static int selectedSpriteIndex = 0;

            // Section to input enemy sprite texture id
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Combo("texture id", &selectedSpriteIndex, sprites, IM_ARRAYSIZE(sprites));
            }
            ImGui::Spacing();

            // Section to input enemy transform values
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("position x", &posX);
                ImGui::InputInt("position y", &posY);
                ImGui::SliderInt("scale x", &scaleX, 1, 10);
                ImGui::SliderInt("scale y", &scaleY, 1, 10);
                ImGui::SliderAngle("rotation (deg)", &rotation, 0, 360);
            }
            ImGui::Spacing();

            // Section to input enemy rigid body values
            if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("velocity x", &velX);
                ImGui::InputInt("velocity y", &velY);
            }
            ImGui::Spacing();

            // Section to input enemy projectile emitter values
            if (ImGui::CollapsingHeader("Projectile emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderAngle("angle (deg)", &projAngle, 0, 360);
                ImGui::SliderFloat("speed (px/sec)", &projSpeed, 10, 500);
                ImGui::InputInt("repeat (sec)", &projRepeat);
                ImGui::InputInt("duration (sec)", &projDuration);
            }
            ImGui::Spacing();

            // Section to input enemy health values
            if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("%", &health, 0, 100);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Spawn new enemy")) {
                Entity enemy = registry->CreateEntity();
                enemy.AddComponent<TransformComponent>(glm::vec2(posX, posY), glm::vec2(scaleX, scaleY),
                                                       glm::degrees(rotation));
                enemy.AddComponent<RigidBodyComponent>(glm::vec2(velX, velY));
                enemy.AddComponent<SpriteComponent>(sprites[selectedSpriteIndex], 32, 32, 2);
                enemy.AddComponent<BoxColliderComponent>(25, 20, glm::vec2(5, 5));
                double projVelX = cos(projAngle) * projSpeed; // convert from angle-speed to x-value
                double projVelY = sin(projAngle) * projSpeed; // convert from angle-speed to y-value
                enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projVelX, projVelY), projRepeat * 1000,
                                                               projDuration * 1000, 10, false);
                enemy.AddComponent<HealthComponent>(health);

                // Reset all input values after we create a new enemy
                posX = posY = rotation = projAngle = 0;
                scaleX = scaleY = 1;
                projRepeat = projDuration = 10;
                projSpeed = 100;
                health = 100;
            }
        }
        ImGui::End();

        // Display a small overlay window to display the map position using the mouse
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.9f);
        if (ImGui::Begin("Map coordinates", nullptr, windowFlags)) {
            ImGui::Text(
                    "Map coordinates (x=%.1f, y=%.1f)",
                    ImGui::GetIO().MousePos.x + camera.x,
                    ImGui::GetIO().MousePos.y + camera.y
            );
        }
        ImGui::End();

        //Render ImGui
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    }
};
