#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"

class DrinkConsumableEvent: public Event {
    public:
        Entity player;
        items itemEnum;
        std::unique_ptr<Registry>& registry;
        std::unique_ptr<AssetStore>& assetstore;
        std::unique_ptr<EventBus>& eventbus;
        int invSlot;
        bool inventory;
        DrinkConsumableEvent(Entity player, items itemEnum, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, std::unique_ptr<EventBus>& eventbus,int invSlot, bool inventory = true): 
            player(player), itemEnum(itemEnum), registry(registry), assetstore(assetstore), eventbus(eventbus), invSlot(invSlot), inventory(inventory){}
};