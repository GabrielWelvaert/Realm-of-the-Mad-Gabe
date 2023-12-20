#ifndef TOMEUSEEVENT_H
#define TOMEUSEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../AssetStore/AssetStore.h"

class TomeUseEvent: public Event {
    public:
        Entity player;
        std::unique_ptr<Registry>& registry;
        std::unique_ptr<AssetStore>& assetstore;
        std::unique_ptr<EventBus>& eventbus;
        int mx;
        int my;
        SDL_Rect camera;
        inline TomeUseEvent(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, std::unique_ptr<EventBus>& eventbus, int mx, int my, SDL_Rect camera): 
            player(player), registry(registry), assetstore(assetstore), eventbus(eventbus), mx(mx), my(my), camera(camera) {}
};

#endif