#include "CollisionSystem.h"

CollisionSystem::CollisionSystem(){
    RequireComponent<BoxColliderComponent>();
    RequireComponent<TransformComponent>();
}

void CollisionSystem::Update(std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, const double& deltaTime, Entity& player, std::unique_ptr<Factory>& factory) {
    auto entities = GetSystemEntities();
    for (auto i = entities.begin(); i != entities.end(); i++){ //sliding window algorithm
        Entity a = *i;
        auto aTransform = a.GetComponent<TransformComponent>();
        auto aCollider = a.GetComponent<BoxColliderComponent>();
        //all entities to the right of i
        for (auto j = i; j != entities.end(); j++){
        
            Entity b = *j;
            if( a==b || playerMonsterCollision(a,b) || bothAreWalls(a,b) || twoMonsters(a,b) || bothAreProjectiles(a,b) || bagAndNotPlayer(a,b) || portalAndNotPlayer(a,b)) {continue;} 
            auto bTransform = b.GetComponent<TransformComponent>();
            auto bCollider = b.GetComponent<BoxColliderComponent>();

            bool projectileandvictim = projectileAndVictim(a,b);
            if(projectileandvictim){ // temporarily increase victim hitbox so they can indeed get headshot
                if(b.BelongsToGroup(PROJECTILE)){
                    aCollider.height += 30; // *= 2.5
                    aCollider.offset[1] -= 30; // /= 2.5
                } else{
                    bCollider.height += 30;
                    bCollider.offset[1] -= 30;
                }
            }

            bool collided = CheckAABBCollision(
                aTransform.position.x + aCollider.offset[0],
                aTransform.position.y + aCollider.offset[1],
                aCollider.width,
                aCollider.height,
                bTransform.position.x + bCollider.offset[0],
                bTransform.position.y + bCollider.offset[1],
                bCollider.width,
                bCollider.height);

            if(collided){
                if(oneEntityIsWall(a,b)){
                    if(oneEntityIsProjectile(a,b)){ // projectile hit wall 
                        a.BelongsToGroup(PROJECTILE) ? a.Kill() : b.Kill();
                    } else { // player/monster hit wall; 
                        a.BelongsToGroup(WALLBOX) ? eventBus->EmitEvent<CollisionEvent>(a,b,getCollisionSide(bTransform.position.x + bCollider.offset[0],bTransform.position.y + bCollider.offset[1],bCollider.width,bCollider.height,aTransform.position.x + aCollider.offset[0],aTransform.position.y + aCollider.offset[1],aCollider.width,aCollider.height)) : eventBus->EmitEvent<CollisionEvent>(b,a,getCollisionSide(aTransform.position.x + aCollider.offset[0],aTransform.position.y + aCollider.offset[1],aCollider.width,aCollider.height,bTransform.position.x + bCollider.offset[0],bTransform.position.y + bCollider.offset[1],bCollider.width,bCollider.height)); //wall should be first parameter
                    }
                } else if (projectileHitSomeone(a,b) && !projectileParentGroupSameAsVictimGroup(a,b)){//no walls; someone was shot!
                    a.BelongsToGroup(PROJECTILE) ? eventBus->EmitEvent<ProjectileDamageEvent>(a,b, eventBus, registry, assetStore, factory) : eventBus->EmitEvent<ProjectileDamageEvent>(b,a, eventBus, registry, assetStore, factory); 
                } else if (playerAndBag(a,b)){ // player collided with loot bag
                    auto& playerIC = player.GetComponent<PlayerItemsComponent>();
                    if(!playerIC.viewingBag){ // only open new bag if not currently viewing a bag
                        if(a.BelongsToGroup(PLAYER)){ // b is the loot bag
                            if(playerIC.viewingPortal){
                                eventBus->EmitEvent<PortalCollisionEvent>(false, a, b, registry); // if player is viewing portal, stop viewing portal; give lootbag priority
                            }
                            if(!b.GetComponent<LootBagComponent>().opened){ // need not open an open bag
                                eventBus->EmitEvent<LootBagCollisionEvent>(b, 11, true, registry, player, eventBus);
                            }
                        } else { // a is the loot bag
                            if(playerIC.viewingPortal){
                                eventBus->EmitEvent<PortalCollisionEvent>(false, b, a, registry); // if player is viewing portal, stop viewing portal; give lootbag priority
                            }
                            if(!a.GetComponent<LootBagComponent>().opened){ // need not open an open bag
                                eventBus->EmitEvent<LootBagCollisionEvent>(a, 11, true, registry, player, eventBus);
                            }
                        }
                    } 
                } else if (playerAndPortal(a,b)){
                    auto& playerIC = player.GetComponent<PlayerItemsComponent>();
                    if(!playerIC.viewingBag && !playerIC.viewingPortal){ // if not already viewing bag or portal
                        if(a.BelongsToGroup(PLAYER)){
                            const auto& area = b.GetComponent<PortalComponent>().area;
                            eventBus->EmitEvent<PortalCollisionEvent>(true, a, b, registry);
                        } else {
                            const auto& area = a.GetComponent<PortalComponent>().area;
                            eventBus->EmitEvent<PortalCollisionEvent>(true, b, a, registry);
                        }
                    }
                }
            }
            
            if(projectileandvictim){
                if(b.BelongsToGroup(PROJECTILE)){
                    aCollider.height -= 30;
                    aCollider.offset[1] += 30;
                } else{
                    bCollider.height -= 30;
                    bCollider.offset[1] += 30;
                }
            }
        }
    }
}