#ifndef PORTALCOLLISIONEVENT_H
#define PORTALCOLLISIONEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Components/PlayerItemsComponent.h"
#include "../EventBus/EventBus.h"
#include "../Utils/enums.h"

class PortalCollisionEvent: public Event{
    public:
        bool status;
        Entity player; // COMPILER WONT LET ME PASS PLAYER BY REFERENCE!!!
        Entity& portal;
        std::unique_ptr<Registry>& registry;

        inline PortalCollisionEvent(bool status, Entity player, Entity& portal, std::unique_ptr<Registry>& registry): 
            status(status), player(player), portal(portal), registry(registry) {}
};

#endif