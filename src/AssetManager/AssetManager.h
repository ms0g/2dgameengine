#pragma once

#include <map>
#include <string>
#include <SDL2/SDL.h>

class AssetManager {
public:
    AssetManager();

    ~AssetManager();

    void ClearAssets();

    void AddTexture(SDL_Renderer* renderer, const std::string& assetID, const std::string& filePath);

    [[nodiscard]] SDL_Texture* GetTexture(const std::string& assetID) const;

private:
    std::map<std::string, SDL_Texture*> textures;
    // fonts
    // audios

};
