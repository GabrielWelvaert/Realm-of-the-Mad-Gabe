#include "ItemIconSystem.h"

ItemIconSystem::ItemIconSystem(){
    RequireComponent<ItemIconComponent>();
}

void ItemIconSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<KillItemIconEvent>(this, &ItemIconSystem::KillIcon);
}

void ItemIconSystem::KillIcon(KillItemIconEvent& event){
    // std::cout << "KillItemIconEvent handled" << std::endl;
    for(auto& entity: GetSystemEntities()){ // kills our single item icon
        entity.Kill();
    }
}

void ItemIconSystem::Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore){
    for(auto& entity: GetSystemEntities()){
        auto& itemIcon = entity.GetComponent<ItemIconComponent>();
        SDL_Rect srcRect = {
            0,
            0,
            static_cast<int>(itemIcon.widthHeight.x), 
            static_cast<int>(itemIcon.widthHeight.y)};

        SDL_Rect dstRect = {
            static_cast<int>(itemIcon.position.x), 
            static_cast<int>(itemIcon.position.y),
            static_cast<int>(itemIcon.widthHeight.x), 
            static_cast<int>(itemIcon.widthHeight.y)};

        SDL_RenderCopyEx( //can pass rotation 
            renderer, 
            assetStore->GetTexture(itemIcon.textureEnum),
            &srcRect,
            &dstRect,
            0.0,
            NULL,
            SDL_FLIP_NONE
        );

    }
}