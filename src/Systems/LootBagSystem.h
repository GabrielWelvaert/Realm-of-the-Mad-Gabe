#pragma once 

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include <vector>
#include "../Utils/tables.h"
#include "../Utils/enums.h"
#include "../Components/LootBagComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"

/*
This system is responsible for manaing lifetime and creation of lootbags and emitting collision events with player
*/

class LootBagSystem: public System{
    private:
        const Uint32 bagLifeTimeMs = 1600000;

        inline bool CheckAABBCollision(double ax, double ay, double aw, double ah, double bx, double by, double bw, double bh){
                    return (
                        ax < bx + bw &&
                        ax + aw > bx &&
                        ay < by + bh &&
                        ay + ah > by
                    );
                }

    public:
        LootBagSystem(){
            RequireComponent<LootBagComponent>();
        }

        // todo: event for spawning loot bag upon monster death 

        // update is respondible for 
        // 1) close an open bag if player is no longer standing on-top 
        // 2) safely kill a bag that has expired
        void Update(Entity player, std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry, PlayerItemsComponent& playerIC){
            for(auto entity: GetSystemEntities()){
                auto& lbc = entity.GetComponent<LootBagComponent>();
                if(lbc.opened){
                    const auto& aCollider = entity.GetComponent<BoxColliderComponent>();
                    const auto& aTransform = entity.GetComponent<TransformComponent>();
                    const auto& bCollider = player.GetComponent<BoxColliderComponent>();
                    const auto& bTransform = player.GetComponent<TransformComponent>();
                    if(!CheckAABBCollision(
                        aTransform.position.x + aCollider.offset[0],
                        aTransform.position.y + aCollider.offset[1],
                        aCollider.width,
                        aCollider.height,
                        bTransform.position.x + bCollider.offset[0],
                        bTransform.position.y + bCollider.offset[1],
                        bCollider.width,
                        bCollider.height)){
                            eventBus->EmitEvent<LootBagCollisionEvent>(entity, 9, false, registry, playerIC);
                        }
                }
                if(SDL_GetTicks() > lbc.spawnTime + bagLifeTimeMs){
                    // todo kill contents and bag iteslf! 
                }
            }
        }

};
