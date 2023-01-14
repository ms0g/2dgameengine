#pragma once

#include <map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

class AssetManager {
public:
    AssetManager();

    ~AssetManager();

    void ClearAssets();

    void AddTexture(SDL_Renderer* renderer, const std::string& assetID, const std::string& filePath);

    void AddFont(const std::string& assetID, const std::string& filePath, int fontSize);

    void AddSound(const std::string& assetID, const std::string& filePath);

    [[nodiscard]] SDL_Texture* GetTexture(const std::string& assetID) const;

    [[nodiscard]] TTF_Font* GetFont(const std::string& assetID) const;

    [[nodiscard]] Mix_Chunk* GetSound(const std::string& assetID) const;

private:
    std::map<std::string, SDL_Texture*> textures;
    std::map<std::string, TTF_Font*> fonts;
    std::map<std::string, Mix_Chunk*> sounds;
    // audios

};
