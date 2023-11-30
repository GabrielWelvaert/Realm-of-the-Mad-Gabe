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
        inline TomeUseEvent(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, std::unique_ptr<EventBus>& eventbus): 
            player(player), registry(registry), assetstore(assetstore), eventbus(eventbus) {}
};

#endif