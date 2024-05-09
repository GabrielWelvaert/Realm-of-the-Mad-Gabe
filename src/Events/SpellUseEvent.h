#ifndef SPELLUSEEVENT_H
#define SPELLUSEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"

class SpellUseEvent: public Event {
    public:
        Entity player;
        std::unique_ptr<Registry>& registry;
        int mx;
        int my;
        SDL_Rect camera;
        inline SpellUseEvent(Entity player, std::unique_ptr<Registry>& registry, int mx, int my, SDL_Rect camera): 
            player(player), registry(registry), mx(mx), my(my), camera(camera) {}

};


#endif