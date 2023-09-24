#pragma once
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/DynamicUIEntityComponent.h"
#include "../Utils/tables.h"

/*
This system is responsible for updating the xp,hp, and mp bars of the UI
*/

// this system updates the parts of the UI that are made of rectangles. 
class DynamicUIRenderSystem: public System{
    private:
        // perhaps more efficient to store 3 vectors
        // each vector contains the hp/mp/txt bar textures at 0% to 100%
        // ex: if hp = 75% use texture at hpBarTextures[75] 
        // rather than do SDL_Render stuff can just access an already existing texture? 
        // this may be slightly more performant. 
    public:

        DynamicUIRenderSystem(){
            RequireComponent<DynamicUIEntityComponent>();
        }

        void Update(SDL_Renderer* renderer, Entity player){ // pass player stats by const &
            const auto& speed = player.GetComponent<SpeedStatComponent>();
            const auto& HPMPstats = player.GetComponent<HPMPComponent>();
            const auto& offenseStats = player.GetComponent<OffenseStatComponent>();
            const auto& baseStats = player.GetComponent<BaseStatComponent>();
            auto entities = GetSystemEntities();

            DynamicUIEntityComponent& lvlbarData = entities[0].GetComponent<DynamicUIEntityComponent>();
            SDL_SetRenderDrawColor(renderer, lvlbarData.r, lvlbarData.g, lvlbarData.b, 255);
            if(baseStats.xp < 18050){
                lvlbarData.rect.w = (static_cast<double>(baseStats.xp) - nextXPToLevelUp[baseStats.level-1]) / (nextXPToLevelUp[baseStats.level] - nextXPToLevelUp[baseStats.level-1] ) * 225;
            } else {
                lvlbarData.rect.w = 225;
            }
            SDL_RenderFillRect(renderer, &lvlbarData.rect);

            DynamicUIEntityComponent& hpbarData = entities[1].GetComponent<DynamicUIEntityComponent>();
            SDL_SetRenderDrawColor(renderer, hpbarData.r, hpbarData.g, hpbarData.b, 255);
            hpbarData.rect.w = HPMPstats.activehp / HPMPstats.maxhp * 225;
            if(hpbarData.rect.w < 0){
                hpbarData.rect.w = 0;
            }
            SDL_RenderFillRect(renderer, &hpbarData.rect);

            DynamicUIEntityComponent& mpbarData = entities[2].GetComponent<DynamicUIEntityComponent>();
            SDL_SetRenderDrawColor(renderer, mpbarData.r, mpbarData.g, mpbarData.b, 255);
            mpbarData.rect.w = HPMPstats.activemp / HPMPstats.maxmp * 225;
            SDL_RenderFillRect(renderer, &mpbarData.rect);

        }
};
