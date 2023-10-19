#pragma once 

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
            RequireComponent<BoxColliderComponent>();
        }

        // update is respondible for 
        // 1) safely kill a bag that has expired
        // 2) close an open bag if player is no longer standing on-top 
        void Update(int my, Entity player, std::unique_ptr<EventBus>& eventBus, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry, PlayerItemsComponent& playerIC){
            for(auto entity: GetSystemEntities()){
                auto& lbc = entity.GetComponent<LootBagComponent>();
                if(SDL_GetTicks() > lbc.spawnTime + bagLifeTimeMs || lbc.contents.size() == 0){ // kill bag and contents! 
                    if(playerIC.IdOfOpenBag == entity.GetId() && playerIC.viewingBag){ // player was viewing this bag; "close" it 
                        if(playerIC.holdingItemLastFrame){
                            // item was hoving over lootbag and not from lootbag OR item was from lootbag
                            if((my > 627 && playerIC.heldItemStartingTransformComp.y < 627) || playerIC.heldItemStartingTransformComp.y > 627){ 
                                playerIC.holdingItemLastFrame = false; 
                                if(registry->HasComponent<TransformComponent>(playerIC.IdOfHeldItem)){
                                    registry->GetComponent<TransformComponent>(playerIC.IdOfHeldItem).position = playerIC.heldItemStartingTransformComp;
                                    assetStore->PlaySound(ERROR);
                                }
                            }
                        } 
                    }
                    lbc.deleteContents(); // kill contents of bag
                    entity.Kill();
                    if(lbc.opened){
                        eventBus->EmitEvent<LootBagCollisionEvent>(entity, 9, false, registry, playerIC);
                    }
                    continue;
                }

                if(lbc.opened){
                    // std::cout << "bag " << entity.GetId() << " open in lootbagsystem w/ " << lbc.contents.size() << " item(s). Player watching bag " << player.GetComponent<PlayerItemsComponent>().IdOfOpenBag << std::endl;
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
                            if(playerIC.holdingItemLastFrame){
                                playerIC.holdingItemLastFrame = false; 
                                if((my > 627 && playerIC.heldItemStartingTransformComp.y < 627) || playerIC.heldItemStartingTransformComp.y > 627){
                                    if(registry->HasComponent<TransformComponent>(playerIC.IdOfHeldItem)){
                                        registry->GetComponent<TransformComponent>(playerIC.IdOfHeldItem).position = playerIC.heldItemStartingTransformComp;
                                        assetStore->PlaySound(ERROR);
                                    }
                                }
                            }
                            eventBus->EmitEvent<LootBagCollisionEvent>(entity, 9, false, registry, playerIC);
                        }
                }
            }
        }

};
