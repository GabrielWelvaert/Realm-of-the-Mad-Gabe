#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"

//bad name but this event is for colliding with hard entities like walls
class CollisionEvent: public Event {
    public:
        Entity a;
        Entity b;
        // std::string side;
        sidehit side;
        double deltatime;
        CollisionEvent(Entity a, Entity b, sidehit side): a(a), b(b), side(side) {};
};

#endif 