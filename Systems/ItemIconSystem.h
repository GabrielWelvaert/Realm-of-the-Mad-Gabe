#ifndef ITEMICONSYSTEM_H
#define ITEMICONSYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Components/ItemIconComponent.h"
#include "../Events/KillItemIconEvent.h"
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../Components/SpriteComponent.h"
#include "../Utils/tables.h"
#include "../Utils/enums.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/ItemComponent.h"

class ItemIconSystem: public System{
    public:
        ItemIconSystem();
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);
        void KillIcon(KillItemIconEvent& event);
        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore);
};


#endif