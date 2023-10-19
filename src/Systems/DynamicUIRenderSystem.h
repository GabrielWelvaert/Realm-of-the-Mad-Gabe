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
#include "../Components/HPMPComponent.h"
#include "../Utils/tables.h"
#include <algorithm>

/*
This system is responsible for updating the xp,hp, and mp bars of the UI
*/

// this system updates the parts of the UI that are made of rectangles. 
class DynamicUIRenderSystem: public System{
    private:

    public:

        DynamicUIRenderSystem(){
            RequireComponent<DynamicUIEntityComponent>();
        }

        void sort(){
            auto& entities = GetSystemEntities();
            std::sort(entities.begin(), entities.end(), [](const Entity& entity1, const Entity& entity2) {return entity1.GetComponent<DynamicUIEntityComponent>().statEnum < entity2.GetComponent<DynamicUIEntityComponent>().statEnum;});
        }

        void Update(SDL_Renderer* renderer, Entity player){ // pass player stats by const &
            auto& entities = GetSystemEntities();
            if(entities.size() == 0){return;}

            const auto& HPMPstats = player.GetComponent<HPMPComponent>();
            const auto& baseStats = player.GetComponent<BaseStatComponent>();

            // correct order according to stat enums is HP,MP,LVL
            // std::sort(entities.begin(), entities.end(), [](const Entity& entity1, const Entity& entity2) {return entity1.GetComponent<DynamicUIEntityComponent>().statEnum < entity2.GetComponent<DynamicUIEntityComponent>().statEnum;});

            DynamicUIEntityComponent& lvlbarData = entities[2].GetComponent<DynamicUIEntityComponent>();
            SDL_SetRenderDrawColor(renderer, lvlbarData.r, lvlbarData.g, lvlbarData.b, 255);
            if(baseStats.xp < 18050){
                lvlbarData.rect.w = (static_cast<double>(baseStats.xp) - nextXPToLevelUp[baseStats.level-1]) / (nextXPToLevelUp[baseStats.level] - nextXPToLevelUp[baseStats.level-1] ) * 225;
            } else {
                lvlbarData.rect.w = 225;
            }
            SDL_RenderFillRect(renderer, &lvlbarData.rect);

            DynamicUIEntityComponent& hpbarData = entities[0].GetComponent<DynamicUIEntityComponent>();
            SDL_SetRenderDrawColor(renderer, hpbarData.r, hpbarData.g, hpbarData.b, 255);
            hpbarData.rect.w = HPMPstats.activehp / HPMPstats.maxhp * 225;
            if(hpbarData.rect.w < 0){
                hpbarData.rect.w = 0;
            }
            SDL_RenderFillRect(renderer, &hpbarData.rect);

            DynamicUIEntityComponent& mpbarData = entities[1].GetComponent<DynamicUIEntityComponent>();
            SDL_SetRenderDrawColor(renderer, mpbarData.r, mpbarData.g, mpbarData.b, 255);
            mpbarData.rect.w = HPMPstats.activemp / HPMPstats.maxmp * 225;
            SDL_RenderFillRect(renderer, &mpbarData.rect);
            

        }
};
