#ifndef ASSETSTORE_H
#define ASSETSTORE_H

#include <unordered_map>
#include <string>
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../../libs/SDL2/SDL_ttf.h"
#include "../../libs/SDL2/SDL_mixer.h"
#include <iostream>
#include "../Utils/enums.h"
#include <vector>

class AssetStore{
    private:
        std::unordered_map<textureEnums, SDL_Texture*> textures; 
        std::unordered_map<std::string, TTF_Font*> fonts; 
        std::unordered_map<soundEnums, Mix_Chunk*> sounds; 
        std::unordered_map<std::string, Mix_Music*> music; 

    public:
        AssetStore();
        ~AssetStore(); 
        void ClearAssets();
        void AddTexture(SDL_Renderer* renderer, const textureEnums& assetId, const std::string& filePath);
        void AddTexture(SDL_Renderer* renderer, const textureEnums& assetId, SDL_Texture * texture);
        SDL_Texture* GetTexture(const textureEnums& assetId);
        void AddFont(const std::string& assetId, const std::string& filePath, const unsigned int& fontSize);
        TTF_Font* GetFont(const std::string& assetId);
        void AddSound(const soundEnums& assetId, const std::string& filePath);
        void PlaySound(const soundEnums& assetId);
        void AddMusic(const std::string& assetId, const std::string& filePath);
        void PlayMusic(const std::string& assetId);
        void DestroyMapTextures();
        void DestroyBackgroundTextures();
        void DestroyGUITextures();


};

#endif 