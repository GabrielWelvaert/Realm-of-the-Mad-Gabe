#ifndef SHIELDUSEEVENT_H
#define SHIELDUSEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"
#include "../EventBus/EventBus.h"

class ShieldUseEvent: public Event{
public:
    Entity player;
    std::unique_ptr<Registry>& registry;
    int mx;
    int my;
    SDL_Rect camera;
    inline ShieldUseEvent(Entity player, std::unique_ptr<Registry>& registry, int mx, int my, SDL_Rect camera): 
        player(player), registry(registry), mx(mx), my(my), camera(camera) {}

};


#endif