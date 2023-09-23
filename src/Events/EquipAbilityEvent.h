#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"

class EquipAbilityEvent: public Event {
    public:
        Entity player;
        items itemEnum;
        EquipAbilityEvent(Entity player, items itemEnum): player(player), itemEnum(itemEnum) {}

};