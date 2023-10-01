#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"
#include "../EventBus/EventBus.h"

class HelmUseEvent: public Event {
    public:
        Entity player;
        std::unique_ptr<EventBus>& eventbus;
        HelmUseEvent(Entity player, std::unique_ptr<EventBus>& eventbus):
            player(player), eventbus(eventbus) {}
};