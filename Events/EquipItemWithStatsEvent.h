#ifndef EQUIPITEMWITHSTATSEVENT_H
#define EQUIPITEMWITHSTATSEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"

class EquipItemWithStatsEvent: public Event{
    public:
        Entity player;
        bool unequip;
        items previousItem;
        bool equip;
        items newItem;

        inline EquipItemWithStatsEvent(bool unequip, items previousItem, bool equip, items newItem, Entity player): unequip(unequip), previousItem(previousItem), equip(equip), newItem(newItem), player(player) {}
        
};

#endif