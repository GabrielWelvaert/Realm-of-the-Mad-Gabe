#include "DynamicUIRenderSystem.h"

DynamicUIRenderSystem::DynamicUIRenderSystem(){
    RequireComponent<DynamicUIEntityComponent>();
}

void DynamicUIRenderSystem::sort(){
    auto& entities = GetSystemEntities();
    std::sort(entities.begin(), entities.end(), [](const Entity& entity1, const Entity& entity2) {return entity1.GetComponent<DynamicUIEntityComponent>().statEnum < entity2.GetComponent<DynamicUIEntityComponent>().statEnum;});
}

void DynamicUIRenderSystem::Update(SDL_Renderer* renderer, Entity player){ // pass player stats by const &
    auto& entities = GetSystemEntities();
    if(entities.size() == 0){return;}

    const auto& HPMPstats = player.GetComponent<HPMPComponent>();
    const auto& baseStats = player.GetComponent<BaseStatComponent>();

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