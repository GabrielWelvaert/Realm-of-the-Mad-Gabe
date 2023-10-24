#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H

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

    inline ItemComponent() = default;

    inline ItemComponent(items itemEnum, unsigned char lastPosition, int IdOfHostBag, sprites bagSpriteEnum = BROWNLOOTBAG)
        : itemEnum(itemEnum), lastPosition(lastPosition), IdOfHostBag(IdOfHostBag) , bagSpriteEnum(bagSpriteEnum) {}

    // used for spawning player and populating their inventory and equipment
    inline ItemComponent(items itemEnum, unsigned char lastPosition)
        : itemEnum(itemEnum), lastPosition(lastPosition), IdOfHostBag(-1) , bagSpriteEnum(itemEnumToLootBagSpriteEnum.at(itemEnum)) {}

};

#endif