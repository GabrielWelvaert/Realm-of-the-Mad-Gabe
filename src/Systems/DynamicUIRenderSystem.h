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
        // std::vector<long> nextXPToLevelUp{0,50,200,450,800,1250,1800,2450,3200,4050,5000,6050,7200,8450,9800,11250,12800,14450,16200,18050};
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
            int i = 0;
            // first 3 are the grey background bars
            for(i; i < 3; i++){
                auto& data = entities[i].GetComponent<DynamicUIEntityComponent>();
                SDL_SetRenderDrawColor(renderer, data.r, data.g, data.b, 255);
                SDL_RenderFillRect(renderer, &data.rect);
            }

            // next 3 are the real, colored bars
            DynamicUIEntityComponent& lvlbarData = entities[3].GetComponent<DynamicUIEntityComponent>();
            SDL_SetRenderDrawColor(renderer, lvlbarData.r, lvlbarData.g, lvlbarData.b, 255);
            if(baseStats.xp < 18050){
                lvlbarData.rect.w = (static_cast<double>(baseStats.xp) - nextXPToLevelUp[baseStats.level-1]) / (nextXPToLevelUp[baseStats.level] - nextXPToLevelUp[baseStats.level-1] ) * 225;
            } else {
                lvlbarData.rect.w = 225;
            }
            SDL_RenderFillRect(renderer, &lvlbarData.rect);

            DynamicUIEntityComponent& hpbarData = entities[4].GetComponent<DynamicUIEntityComponent>();
            SDL_SetRenderDrawColor(renderer, hpbarData.r, hpbarData.g, hpbarData.b, 255);
            hpbarData.rect.w = HPMPstats.activehp / HPMPstats.maxhp * 225;
            if(hpbarData.rect.w < 0){
                hpbarData.rect.w = 0;
            }
            SDL_RenderFillRect(renderer, &hpbarData.rect);

            DynamicUIEntityComponent& mpbarData = entities[5].GetComponent<DynamicUIEntityComponent>();
            SDL_SetRenderDrawColor(renderer, mpbarData.r, mpbarData.g, mpbarData.b, 255);
            mpbarData.rect.w = HPMPstats.activemp / HPMPstats.maxmp * 225;
            SDL_RenderFillRect(renderer, &mpbarData.rect);

        }
};
