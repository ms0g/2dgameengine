#pragma once

#include <SDL2/SDL_mixer.h>
#include "../ECS/ECS.h"
#include "../Components/SoundComponent.hpp"

class SoundSystem: public System {
public:
    SoundSystem() {
        RequireComponent<SoundComponent>();
    }

    void Update(const std::unique_ptr<AssetManager>& assetManager) {
        for (const auto& entity: GetSystemEntities()) {
            const auto& sound = entity.GetComponent<SoundComponent>();
            Mix_PlayChannel(-1, assetManager->GetSound(sound.assetID), sound.loopCount);
        }
    }

};
