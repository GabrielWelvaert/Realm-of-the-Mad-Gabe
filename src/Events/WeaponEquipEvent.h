#ifndef WEAPONEQUIPEVENT_H
#define WEAPONEQUIPEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"

class WeaponEquipEvent: public Event{
    public:
        items itemEnum;
        Entity player;
        inline WeaponEquipEvent(items itemEnum, Entity player): itemEnum(itemEnum), player(player) {}
};

#endif