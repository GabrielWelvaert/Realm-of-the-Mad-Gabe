#pragma once

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <map>  
#include "../Components/ItemComponent.h"

struct PlayerItemsComponent{
    std::map<unsigned char, Entity> equipment;
    std::map<unsigned char, Entity> inventory;
    std::map<unsigned char, Entity>* ptrToOpenBag = nullptr; // points to nullptr if no bag is open...

    PlayerItemsComponent() = default;
};