#include "AssetManager.h"
#include <SDL2/SDL_image.h>
#include "../Logger/Logger.h"


AssetManager::AssetManager() {
    Logger::Log("Asset Manager ctor called!");
}

AssetManager::~AssetManager() {
    ClearAssets();
    Logger::Log("Asset Manager dtor called!");

}

void AssetManager::ClearAssets() {
    for (auto& texture: textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();
}

void AssetManager::AddTexture(SDL_Renderer* renderer, const std::string& assetID, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    textures.emplace(assetID, texture);
    Logger::Log("New texture added to Asset Store with ID = " + assetID);
}

SDL_Texture* AssetManager::GetTexture(const std::string& assetID) const {
    return textures.at(assetID);
}
