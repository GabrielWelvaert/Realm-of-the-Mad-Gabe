#pragma once

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <map>  
#include "../Components/ItemComponent.h"

struct PlayerItemsComponent{
    std::map<unsigned char, Entity> equipment;
    std::map<unsigned char, Entity> inventory;
    int IdOfOpenBag;
    bool viewingBag;
    bool holdingItemLastFrame = false;
    int IdOfHeldItem = 0;
    glm::vec2 heldItemStartingTransformComp;

    PlayerItemsComponent() = default;

    void KillPlayerItems(){
        for(auto& x: equipment){
            x.second.Kill();
        }
        equipment.clear();
        for(auto& x: inventory){
            x.second.Kill();
        }
        inventory.clear();
    }
};