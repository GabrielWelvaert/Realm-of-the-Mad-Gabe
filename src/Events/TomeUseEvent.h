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
        inline TomeUseEvent(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore): 
            player(player), registry(registry), assetstore(assetstore) {}
};

#endif