#pragma once 

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"

class StatusEffectEvent: public Event {
    public:
        Entity recipient;
        statuses statusEffectEnum;
        Uint32 duration;
        std::unique_ptr<EventBus>& eventbus;
        std::unique_ptr<Registry>& registry;
        StatusEffectEvent(Entity recipient, statuses statusEffectEnum, std::unique_ptr<EventBus>& eventbus, std::unique_ptr<Registry>& registry, Uint32 duration = 0): 
            recipient(recipient), 
            statusEffectEnum(statusEffectEnum),  
            eventbus(eventbus),
            registry(registry),
            duration(duration)
            {}

};