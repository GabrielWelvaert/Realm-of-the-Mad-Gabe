#include "AssetStore.h"

AssetStore::AssetStore(){
    // std::cout << "assetstore constructor called" << std::endl;
}

AssetStore::~AssetStore(){
    ClearAssets();
    // std::cout << "assetstore destructor called" << std::endl;
}

void AssetStore::ClearAssets(){
    for (auto texture: textures){
        SDL_DestroyTexture(texture.second);
    }
    for (auto font: fonts){
        TTF_CloseFont(font.second);
    }
    for (auto sound: sounds){
        Mix_FreeChunk(sound.second);
    }
    for (auto song: music){
        Mix_FreeMusic(song.second);
    }
    textures.clear();
    fonts.clear();
    sounds.clear();
    music.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const textureEnums& assetId, const std::string filePath){
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);    
    textures.emplace(assetId, texture);
    // std::cout << SDL_GetError() << std::endl;
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const textureEnums& assetId, SDL_Texture * texture){
    textures.emplace(assetId, texture);
}

SDL_Texture* AssetStore::GetTexture(const textureEnums& assetId){
    if(textures.find(assetId) == textures.end()){
        std::cout << assetId << " not found in assetStore's texures. Perhaps there was a typo! " << std::endl;
    }
    return textures.at(assetId);
}

void AssetStore::AddFont(const std::string& assetId, const std::string filePath, unsigned int fontSize){
    fonts.emplace(assetId, TTF_OpenFont(filePath.c_str(), fontSize));

}

TTF_Font* AssetStore::GetFont(const std::string& assetId){
    if(fonts.find(assetId) == fonts.end()){
        // std::cout << assetId << " not found in assetStore's fonts. Perhaps there was a typo! " << std::endl;
    }
    return fonts.at(assetId);
}

void AssetStore::AddSound(const soundEnums& assetId, const std::string& filePath){
    sounds.emplace(assetId, Mix_LoadWAV(filePath.c_str()));
    // std::cout << assetId << " added to asset store " << std::endl;
    // if(SDL_GetError() != ""){    std::cout << SDL_GetError() << std::endl; }
    
}

void AssetStore::PlaySound(const soundEnums& assetId){
    if(sounds.find(assetId) == sounds.end()){
        // std::cout << assetId << " not found in assetStore's sounds. Perhaps there was a typo! " << std::endl;
    }
    Mix_PlayChannel(-1, sounds.at(assetId), 0);
}

void AssetStore::AddMusic(const std::string& assetId, const std::string& filePath){
    music.emplace(assetId, Mix_LoadMUS(filePath.c_str()));
}

void AssetStore::PlayMusic(const std::string& assetId){
    if(!Mix_PlayingMusic()){
        if(music.find(assetId) == music.end()){
            // std::cout << assetId << " not found in assetStore's music. Perhaps there was a typo! " << std::endl;
        }
        Mix_PlayMusic(music.at(assetId), -1);
    } else {
        Mix_HaltMusic();
    }
}


