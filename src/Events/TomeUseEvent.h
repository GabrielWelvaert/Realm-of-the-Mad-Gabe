#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"

class TomeUseEvent: public Event {
    public:
        Entity player;
        TomeUseEvent(Entity player): player(player) {}
};