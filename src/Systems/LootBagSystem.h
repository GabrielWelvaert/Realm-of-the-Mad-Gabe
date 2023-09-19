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
This system is responsible for 
1) killing expired bags 
2) spawning lootbags on event (dropping item, killing entity that dropped a lootbag)
3) closing lootbags that are no longer collided with (note, opening detected in collision system and done in InteractUISystem::displayBag)
*/

class LootBagSystem: public System{
    private:
        const Uint32 bagLifeTimeMs = 5000;

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
        void Update(int my, Entity player, std::unique_ptr<EventBus>& eventBus, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry, PlayerItemsComponent& playerIC){
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
                if(SDL_GetTicks() > lbc.spawnTime + bagLifeTimeMs){ // kill bag and contents! 
                    if(playerIC.ptrToOpenBag == &lbc.contents){ // player was viewing this bag; "close" it 
                        eventBus->EmitEvent<LootBagCollisionEvent>(entity, 9, false, registry, playerIC);
                        if(playerIC.holdingItemLastFrame){
                            if(!lbc.hasItem(playerIC.IdOfHeldItem)){ // held item not from loot bag
                                if(my > 625){ // if player was attempting to drop item into bag but it despawned:
                                    if(playerIC.IdOfHeldItem != 0){ // safe safe safe
                                        registry->GetComponent<TransformComponent>(playerIC.IdOfHeldItem).position = playerIC.heldItemStartingTransformComp;
                                        assetStore->PlaySound(ERROR);    
                                    }
                                }
                            } else { // held item from loot bag
                                assetStore->PlaySound(ERROR);
                            }
                        } 
                    }
                    playerIC.holdingItemLastFrame = false;
                    lbc.deleteContents(); // kill contents of bag
                    entity.Kill();
                }
            }
        }

};
