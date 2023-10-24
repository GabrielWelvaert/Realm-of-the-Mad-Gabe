#ifndef UPDATEDISPLAYSTATEVENT_H
#define UPDATEDISPLAYSTATEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class UpdateDisplayStatEvent: public Event{
public:
    Entity player;
    inline UpdateDisplayStatEvent(Entity player): player(player) {}
};

#endif