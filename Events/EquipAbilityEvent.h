#ifndef EQUIPABILITYEVENT_H
#define EQUIPABILITYEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"

class EquipAbilityEvent: public Event {
    public:
        Entity player;
        items itemEnum;
        inline EquipAbilityEvent(Entity player, items itemEnum): player(player), itemEnum(itemEnum) {}

};

#endif