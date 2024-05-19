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
        sidehit side;
        double deltatime;
        std::unique_ptr<Registry>& registry;
        inline CollisionEvent(Entity a, Entity b, sidehit side, std::unique_ptr<Registry>& registry): 
            a(a), b(b), side(side), registry(registry) {};
};

#endif 