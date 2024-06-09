#ifndef DRINKCONSUMABLEEVENT_H
#define DRINKCONSUMABLEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"
#include "../Utils/factory.h"

class DrinkConsumableEvent: public Event {
    public:
        Entity player;
        items itemEnum;
        std::unique_ptr<Registry>& registry;
        std::unique_ptr<AssetStore>& assetstore;
        std::unique_ptr<EventBus>& eventbus;
        std::unique_ptr<Factory>& factory;
        int invSlot;
        bool inventory;
        inline DrinkConsumableEvent(Entity player, items itemEnum, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, std::unique_ptr<EventBus>& eventbus,std::unique_ptr<Factory>& factory,int invSlot, bool inventory = true): 
            player(player), itemEnum(itemEnum), registry(registry), assetstore(assetstore), eventbus(eventbus), factory(factory),invSlot(invSlot), inventory(inventory){}
};

#endif