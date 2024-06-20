#ifndef UPDATEDISPLAYSTATTEXTSYSTEM_H
#define UPDATEDISPLAYSTATTEXTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/DisplayStatComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Utils/enums.h"
#include "../EventBus/EventBus.h"
#include "../Events/UpdateDisplayStatEvent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../../libs/SDL2/SDL_ttf.h"
#include "../Utils/colors.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/TransformComponent.h"
#include "../AssetStore/AssetStore.h"
#include <algorithm>

/*
This system updates the display strings for xp, hp, and mp. They are only displayed and updated when mouse pos > 750
They are rendered in RenderTextSystem
*/

class UpdateDisplayStatTextSystem: public System{
    private:
        bool wasDisplayingPotsToMaxLastFrame = false;
        
    public:
        UpdateDisplayStatTextSystem();

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);

        void onUpdateDisplayName();

        void sortEntities();

        // event update for att,def,spd,dex,vit,wis and display string
        void onDisplayStatUpdate(UpdateDisplayStatEvent& event);

        void checkMaxHPMP(Entity player);

        // system update each frame if mousepos > 750 for xp, hp, mp display string
        void Update(int mx, int my, Entity player, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, std::unique_ptr<EventBus>& eventBus);


};




#endif