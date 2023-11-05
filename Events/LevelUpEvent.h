#ifndef LEVELUPEVENT_H
#define LEVELUPEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../EventBus/EventBus.h"

class LevelUpEvent: public Event{
public:
    Entity player;
    std::unique_ptr<Registry>& registry;
    std::unique_ptr<EventBus>& eventBus;
    inline LevelUpEvent(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<EventBus>& eventBus): player(player), registry(registry), eventBus(eventBus) {}

};

#endif