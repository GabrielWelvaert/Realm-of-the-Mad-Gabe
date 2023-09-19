
#ifndef PROJECTILEDAMAGEEVENT_H
#define PROJECTILEDAMAGEEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include "../EventBus/EventBus.h"
#include "../Events/LevelUpEvent.h"

class ProjectileDamageEvent : public Event {
public:
    Entity projectile;
    Entity victim;
    std::unique_ptr<EventBus>& eventBus; // this event can emit further events...
    std::unique_ptr<Registry>& registry; // bit hacky, but this event has registry access...
    std::unique_ptr<AssetStore>& assetStore; // is this bad lol

    ProjectileDamageEvent(Entity projectile, Entity victim, std::unique_ptr<EventBus>& eventBus,std::unique_ptr<Registry>& registry,std::unique_ptr<AssetStore>& assetStore)
        : projectile(projectile), victim(victim), eventBus(eventBus), registry(registry), assetStore(assetStore){
    }
};



#endif