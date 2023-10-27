
#ifndef PROJECTILEDAMAGEEVENT_H
#define PROJECTILEDAMAGEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../EventBus/EventBus.h"
#include "../Events/LevelUpEvent.h"
#include "../AssetStore/AssetStore.h"
#include "../Utils/factory.h"

class ProjectileDamageEvent : public Event {
public:
    Entity projectile;
    Entity victim;
    std::unique_ptr<EventBus>& eventBus; 
    std::unique_ptr<Registry>& registry;
    std::unique_ptr<AssetStore>& assetStore; 
    std::unique_ptr<Factory>& factory;

    inline ProjectileDamageEvent(Entity projectile, Entity victim, std::unique_ptr<EventBus>& eventBus,std::unique_ptr<Registry>& registry,std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Factory>& factory)
        : projectile(projectile), victim(victim), eventBus(eventBus), registry(registry), assetStore(assetStore), factory(factory){
    }
};

#endif