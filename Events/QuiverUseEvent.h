#ifndef QUIVERUSEEVENT_H
#define QUIVERUSEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"

class QuiverUseEvent: public Event {
    public:
        Entity player;
        std::unique_ptr<Registry>& registry;
        int mx;
        int my;
        SDL_Rect camera;
        inline QuiverUseEvent(Entity player, std::unique_ptr<Registry>& registry, int mx, int my, SDL_Rect camera): 
            player(player), registry(registry), mx(mx), my(my), camera(camera) {}
};

#endif