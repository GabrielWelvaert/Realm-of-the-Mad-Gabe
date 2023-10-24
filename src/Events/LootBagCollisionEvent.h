#ifndef LOOTBAGCOLLISIONEVENT_H
#define LOOTBAGCOLLISIONEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Components/PlayerItemsComponent.h"

class LootBagCollisionEvent: public Event{
    public:
        Entity lootbag;
        unsigned char zIndex;
        bool status;
        std::unique_ptr<Registry>& registry;
        PlayerItemsComponent& playerIC;
        inline LootBagCollisionEvent(Entity lootbag, unsigned char zIndex, bool status, std::unique_ptr<Registry>& registry, PlayerItemsComponent& playerIC): 
            lootbag(lootbag), zIndex(zIndex), status(status), registry(registry), playerIC(playerIC) {}
};

#endif