#ifndef STATUSEFFECTEVENT_H
#define STATUSEFFECTEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"
#include "../EventBus/EventBus.h"

class StatusEffectEvent: public Event {
    public:
        Entity recipient;
        statuses statusEffectEnum;
        Uint32 duration;
        std::unique_ptr<EventBus>& eventbus;
        std::unique_ptr<Registry>& registry;
        inline StatusEffectEvent(Entity recipient, statuses statusEffectEnum, std::unique_ptr<EventBus>& eventbus, std::unique_ptr<Registry>& registry, Uint32 duration = 0): 
            recipient(recipient), 
            statusEffectEnum(statusEffectEnum),  
            eventbus(eventbus),
            registry(registry),
            duration(duration)
            {}

};

#endif