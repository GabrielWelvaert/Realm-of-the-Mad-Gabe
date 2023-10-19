#pragma once 

#include "../../libs/SDL2/SDL.h"
#include "../ECS/ECS.h"
#include <memory>
#include <map>
#include "../Utils/enums.h"

struct ItemComponent{
    items itemEnum;
    unsigned char lastPosition;
    int IdOfHostBag;
    sprites bagSpriteEnum;

    ItemComponent() = default;

    ItemComponent(items itemEnum, unsigned char lastPosition, int IdOfHostBag, sprites bagSpriteEnum = BROWNLOOTBAG)
        : itemEnum(itemEnum), lastPosition(lastPosition), IdOfHostBag(IdOfHostBag) , bagSpriteEnum(bagSpriteEnum) {}

    // used for spawning player and populating their inventory and equipment
    ItemComponent(items itemEnum, unsigned char lastPosition)
        : itemEnum(itemEnum), lastPosition(lastPosition), IdOfHostBag(-1) , bagSpriteEnum(itemEnumToLootBagSpriteEnum.at(itemEnum)) {}

};