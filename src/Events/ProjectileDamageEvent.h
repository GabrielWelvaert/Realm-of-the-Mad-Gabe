
#ifndef PROJECTILEDAMAGEEVENT_H
#define PROJECTILEDAMAGEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../EventBus/EventBus.h"
#include "../Events/LevelUpEvent.h"
#include "../AssetStore/AssetStore.h"
#include "../Utils/factory.h"
#include "../Utils/DeadPlayer.h"

class ProjectileDamageEvent : public Event {
public:
    Entity projectile;
    Entity victim;
    std::unique_ptr<EventBus>& eventBus; 
    std::unique_ptr<Registry>& registry;
    std::unique_ptr<AssetStore>& assetStore; 
    std::unique_ptr<Factory>& factory;
    std::function<void(bool, bool, wallTheme)> Setup;
    DeadPlayer * dp;
    std::string activeCharacterID;
    CharacterManager * characterManager;
    int * skullTracker = nullptr;

    inline ProjectileDamageEvent(Entity projectile, Entity victim, std::unique_ptr<EventBus>& eventBus,std::unique_ptr<Registry>& registry,std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Factory>& factory,std::function<void(bool, bool, wallTheme)> Setup, DeadPlayer& dp, std::string& activeCharacterID, std::unique_ptr<CharacterManager>& characterManager)
        : projectile(projectile), victim(victim), eventBus(eventBus), registry(registry), assetStore(assetStore), factory(factory), Setup(Setup), dp(&dp), activeCharacterID(activeCharacterID), characterManager(characterManager.get()){
    }

    // this version is ONLY SAFE TO USE IF THE VICTIM IS A MONSTER! NEVER USE IF VICTIM IS THE PLAYER!
    inline ProjectileDamageEvent(Entity projectile, Entity victim, std::unique_ptr<EventBus>& eventBus,std::unique_ptr<Registry>& registry,std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Factory>& factory, int * skullTracker = nullptr):
        projectile(projectile),
        victim(victim),
        eventBus(eventBus),
        registry(registry),
        assetStore(assetStore),
        factory(factory),
        Setup(nullptr),
        dp(nullptr),
        activeCharacterID(""),
        characterManager(nullptr),
        skullTracker(skullTracker)
        {}

};

#endif