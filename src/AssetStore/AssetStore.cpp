#include "AssetStore.h"

AssetStore::AssetStore(){
}

AssetStore::~AssetStore(){
    ClearAssets();
}

void AssetStore::ClearAssets(){
    for(auto texture: textures){
        SDL_DestroyTexture(texture.second);
    }
    for(auto font: fonts){
        TTF_CloseFont(font.second);
    }
    for(auto sound: sounds){
        Mix_FreeChunk(sound.second);
    }
    for(auto song: music){
        Mix_FreeMusic(song.second);
    }
    textures.clear();
    fonts.clear();
    sounds.clear();
    music.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const textureEnums& assetId, const std::string& filePath){
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);    
    textures.insert_or_assign(assetId, texture);
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const textureEnums& assetId, SDL_Texture * texture){
    if(textures.find(assetId) != textures.end()){
        SDL_DestroyTexture(textures.at(assetId));
    }
    textures.insert_or_assign(assetId, texture);

}

SDL_Texture* AssetStore::GetTexture(const textureEnums& assetId){
    if(textures.find(assetId) == textures.end()){
        std::cout << assetId << " not found in assetStore's texures. Perhaps there was a typo! " << std::endl;
    }
    return textures.at(assetId);
}

void AssetStore::AddFont(const std::string& assetId, const std::string& filePath, const unsigned int& fontSize){
    fonts.emplace(assetId, TTF_OpenFont(filePath.c_str(), fontSize));

}

TTF_Font* AssetStore::GetFont(const std::string& assetId){
    return fonts.at(assetId);
}

void AssetStore::AddSound(const soundEnums& assetId, const std::string& filePath){
    sounds.emplace(assetId, Mix_LoadWAV(filePath.c_str()));
    // std::cout << assetId << " added to asset store " << std::endl;
    // if(SDL_GetError() != ""){    std::cout << SDL_GetError() << std::endl; }
    
}

void AssetStore::PlaySound(const soundEnums& assetId){
    Mix_PlayChannel(-1, sounds.at(assetId), 0);
}

void AssetStore::AddMusic(const std::string& assetId, const std::string& filePath){
    music.emplace(assetId, Mix_LoadMUS(filePath.c_str()));
}

void AssetStore::PlayMusic(const std::string& assetId){
    if(!Mix_PlayingMusic()){
        Mix_PlayMusic(music.at(assetId), -1);
    } else {
        Mix_HaltMusic();
    }
}


