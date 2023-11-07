#ifndef LOOTBAGCOLLISIONEVENT_H
#define LOOTBAGCOLLISIONEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Components/PlayerItemsComponent.h"
#include "../EventBus/EventBus.h"

class LootBagCollisionEvent: public Event{
    public:
        Entity& lootbag;
        unsigned char zIndex;
        bool status;
        std::unique_ptr<Registry>& registry;
        Entity& player;
        std::unique_ptr<EventBus>& eventBus;
        inline LootBagCollisionEvent(Entity& lootbag, unsigned char zIndex, bool status, std::unique_ptr<Registry>& registry, Entity& player, std::unique_ptr<EventBus>& eventBus): 
            lootbag(lootbag), zIndex(zIndex), status(status), registry(registry), player(player), eventBus(eventBus){}
};

#endif