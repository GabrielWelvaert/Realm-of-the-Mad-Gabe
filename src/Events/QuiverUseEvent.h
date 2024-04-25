#ifndef QUIVERUSEEVENT_H
#define QUIVERUSEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"
#include "../EventBus/EventBus.h"

class QuiverUseEvent: public Event {
    public:
        Entity player;
        std::unique_ptr<Registry>& registry;
        int mx;
        int my;
        SDL_Rect camera;
        std::unique_ptr<EventBus>& eventbus;
        inline QuiverUseEvent(Entity player, std::unique_ptr<Registry>& registry, int mx, int my, SDL_Rect camera, std::unique_ptr<EventBus>& eventbus): 
            player(player), registry(registry), mx(mx), my(my), camera(camera), eventbus(eventbus) {}
};

#endif