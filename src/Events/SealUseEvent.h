#ifndef SEALUSEEVENT_H
#define SEALUSEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"
#include "../EventBus/EventBus.h"

class SealUseEvent: public Event {
    public:
        Entity player;
        std::unique_ptr<EventBus>& eventbus;
        std::unique_ptr<Registry>& registry;
        inline SealUseEvent(Entity player, std::unique_ptr<EventBus>& eventbus, std::unique_ptr<Registry>& registry):
            player(player), eventbus(eventbus), registry(registry) {}
};

#endif