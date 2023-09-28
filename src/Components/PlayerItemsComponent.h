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
};