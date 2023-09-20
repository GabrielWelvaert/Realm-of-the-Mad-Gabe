#pragma once 

#include "../../libs/SDL2/SDL.h"
#include "../ECS/ECS.h"
#include <memory>
#include <map>

struct ItemComponent{
    items itemEnum;
    unsigned char lastPosition;
    std::map<unsigned char, Entity>* hostMap;

    ItemComponent() = default;

    ItemComponent(items itemEnum, unsigned char lastPosition, std::map<unsigned char, Entity>& hostMap)
        : itemEnum(itemEnum), lastPosition(lastPosition), hostMap(&hostMap) {}

};