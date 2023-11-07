#include "LootBagSystem.h"

LootBagSystem::LootBagSystem(){
    RequireComponent<LootBagComponent>();
    RequireComponent<BoxColliderComponent>();
    RequireComponent<TransformComponent>();
}

void LootBagSystem::Update(int my, Entity& player, std::unique_ptr<EventBus>& eventBus, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry, wallTheme area){
    auto& playerIC = player.GetComponent<PlayerItemsComponent>();
    for(auto entity: GetSystemEntities()){
        auto& lbc = entity.GetComponent<LootBagComponent>();
        if(area != VAULT && (SDL_GetTicks() > lbc.spawnTime + bagLifeTimeMs || lbc.contents.size() == 0)){ // kill bag and contents! 
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
                eventBus->EmitEvent<LootBagCollisionEvent>(entity, 9, false, registry, player, eventBus);
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
                        // playerIC.holdingItemLastFrame = false; 
                        if((my > 627 && playerIC.heldItemStartingTransformComp.y < 627) || playerIC.heldItemStartingTransformComp.y > 627){
                            if(registry->HasComponent<TransformComponent>(playerIC.IdOfHeldItem)){
                                playerIC.holdingItemLastFrame = false; //moved to here?
                                registry->GetComponent<TransformComponent>(playerIC.IdOfHeldItem).position = playerIC.heldItemStartingTransformComp;
                                assetStore->PlaySound(ERROR);
                            }
                        }
                    }
                    eventBus->EmitEvent<LootBagCollisionEvent>(entity, 9, false, registry, player, eventBus);
                }
        }
    }
}