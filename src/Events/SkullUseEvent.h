#ifndef SKULLUSEEVENT_H
#define SKULLUSEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"
#include "../EventBus/EventBus.h"
#include "../Utils/factory.h"

class SkullUseEvent: public Event{
public:
    Entity player;
    std::unique_ptr<Registry>& registry;
    std::unique_ptr<Factory>& factory;
    std::unique_ptr<EventBus>& eventBus;
    std::unique_ptr<AssetStore>& assetStore;
    int mx;
    int my;
    SDL_Rect camera;
    inline SkullUseEvent(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory, int mx, int my, SDL_Rect camera, std::unique_ptr<EventBus>& eventBus, std::unique_ptr<AssetStore>& assetStore): 
        player(player), registry(registry), factory(factory), mx(mx), my(my), camera(camera), eventBus(eventBus), assetStore(assetStore) {}

};

#endif