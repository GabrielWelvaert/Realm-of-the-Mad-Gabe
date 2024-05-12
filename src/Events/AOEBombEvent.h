#ifndef AOEBOMBEVENT_H
#define AOEBOMBEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"
#include "../EventBus/EventBus.h"
#include "../../libs/glm/glm.hpp"
#include "../Utils/factory.h"
#include "../AssetStore/AssetStore.h"
#include "../Utils/deadPlayer.h"

class AOEBombEvent: public Event {
    public:
        sprites spriteOfEmitter;
        Entity player;
        Entity projectile; // the AOE bomb, which dies at the end of this frame
        glm::vec2 epicenter;
        std::unique_ptr<EventBus>& eventBus;
        float radius;

        std::unique_ptr<Registry>& registry;
        std::unique_ptr<AssetStore>& assetStore; 
        std::unique_ptr<Factory>& factory;
        std::function<void(bool, bool, wallTheme)> Setup;
        deadPlayer& dp;
        std::string& activeCharacterID;
        std::unique_ptr<CharacterManager>& characterManager;

        inline AOEBombEvent(sprites spriteOfEmitter,
                    Entity player, 
                    Entity projectile, 
                    glm::vec2 epicenter,
                    float radius,
                    std::unique_ptr<EventBus>& eventBus,
                    std::unique_ptr<Registry>& registry,
                    std::unique_ptr<AssetStore>& assetStore, 
                    std::unique_ptr<Factory>& factory,
                    std::function<void(bool, bool, wallTheme)> Setup, 
                    deadPlayer& dp, 
                    std::string& activeCharacterID, 
                    std::unique_ptr<CharacterManager>& characterManager) 
                : spriteOfEmitter(spriteOfEmitter),
                player(player),
                projectile(projectile),
                epicenter(epicenter),
                radius(radius),
                eventBus(eventBus),
                registry(registry),
                assetStore(assetStore),
                factory(factory),
                Setup(Setup),
                dp(dp),
                activeCharacterID(activeCharacterID),
                characterManager(characterManager) {
            }

};

#endif