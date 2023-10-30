#ifndef LOOTBAGSYSTEM_H
#define LOOTBAGSYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <vector>
#include "../Utils/tables.h"
#include "../Utils/enums.h"
#include "../Components/LootBagComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Events/LootBagCollisionEvent.h"
#include "../EventBus/EventBus.h"
#include "../AssetStore/AssetStore.h"

/*
This system is responsible for 
1) killing expired bags 
2) spawning lootbags on event (dropping item, killing entity that dropped a lootbag)
3) closing lootbags that are no longer collided with (note, opening detected in collision system and done in InteractUISystem::displayBag)
*/

class LootBagSystem: public System{
    private:
        const Uint32 bagLifeTimeMs = 30000;

        inline bool CheckAABBCollision(double ax, double ay, double aw, double ah, double bx, double by, double bw, double bh){
            return (
                ax < bx + bw &&
                ax + aw > bx &&
                ay < by + bh &&
                ay + ah > by
            );
        }

    public:
        LootBagSystem();

        // update is respondible for 
        // 1) safely kill a bag that has expired
        // 2) close an open bag if player is no longer standing on-top 
        void Update(int my, Entity& player, std::unique_ptr<EventBus>& eventBus, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry);

};

#endif