#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class LevelUpEvent: public Event{
public:
    Entity player;
    std::unique_ptr<Registry>& registry;
    std::unique_ptr<EventBus>& eventBus;
    LevelUpEvent(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<EventBus>& eventBus): player(player), registry(registry), eventBus(eventBus) {}

};
