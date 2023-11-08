#ifndef UPDATEDISPLAYNAME_H
#define UPDATEDISPLAYNAME_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../EventBus/EventBus.h"
#include "../Utils/CharacterManager.h"
#include "../AssetStore/AssetStore.h"

class UpdateDisplayNameEvent: public Event{
    public:
        Entity player;
        std::unique_ptr<Registry>& registry;
        std::function<void()> gameRender;
        std::unique_ptr<CharacterManager>& characterManager;
        std::unique_ptr<AssetStore>& assetstore;
        inline UpdateDisplayNameEvent(Entity player, std::unique_ptr<Registry>& registry, std::function<void()> gameRender, std::unique_ptr<CharacterManager>& characterManager, std::unique_ptr<AssetStore>& assetstore): 
            player(player), registry(registry), gameRender(gameRender), characterManager(characterManager), assetstore(assetstore) {}
};

#endif