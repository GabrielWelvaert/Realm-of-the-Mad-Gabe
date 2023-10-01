#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"

class TomeUseEvent: public Event {
    public:
        Entity player;
        std::unique_ptr<Registry>& registry;
        std::unique_ptr<AssetStore>& assetstore;
        TomeUseEvent(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore): 
            player(player), registry(registry), assetstore(assetstore) {}
};