#pragma once 

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class UpdateDisplayStatEvent: public Event{
public:
    Entity player;
    UpdateDisplayStatEvent(Entity player): player(player) {}
};