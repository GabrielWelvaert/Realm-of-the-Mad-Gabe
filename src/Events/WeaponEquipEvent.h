#pragma once 

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../utils/enums.h"

class WeaponEquipEvent: public Event{
    public:
        items itemEnum;
        Entity player;
        WeaponEquipEvent(items itemEnum, Entity player): itemEnum(itemEnum), player(player) {}
};