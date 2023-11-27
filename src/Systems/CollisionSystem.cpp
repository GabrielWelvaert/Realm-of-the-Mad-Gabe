#include "CollisionSystem.h"

CollisionSystem::CollisionSystem(){
    RequireComponent<BoxColliderComponent>();
    RequireComponent<TransformComponent>();
}

void CollisionSystem::Update(std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, const double& deltaTime, std::unique_ptr<Factory>& factory, const SDL_Rect& camera, std::function<void(bool, bool, wallTheme)> Setup, deadPlayer& deadPlayer, std::string& activeCharacterID, std::unique_ptr<CharacterManager>& characterManager) {
    auto& entities = GetSystemEntities();
    const int x = 500; // 500 pixels off camera is around 8 tiles past the bounds of the camera
    for(auto i = entities.begin(); i != entities.end(); i++){
        Entity& a = *i;
        auto& aTransform = a.GetComponent<TransformComponent>();
        auto& aCollider = a.GetComponent<BoxColliderComponent>();
        // skip entities which are X pixels off-screen
        if((aTransform.position.x + aCollider.width < camera.x - x|| 
            aTransform.position.y + aCollider.height < camera.y - x|| 
            aTransform.position.x > camera.x + camera.w + x|| 
            aTransform.position.y > camera.y + camera.h + x)){    
            continue;
        }
        // for all entities to the right of i (sliding window algorithm)
        for(auto j = i; j != entities.end(); j++){
            Entity& b = *j;
            if(a==b){continue;}
            groups Agroup = registry->IdToGroup(a.GetId());
            groups Bgroup = registry->IdToGroup(b.GetId());

            // switch case for continue cases (avoids checking AABB for cases where collision doens't matter)
            switch(Agroup){
                case PLAYER:{ // Entity A is player
                    switch(Bgroup){
                        case MONSTER:{
                            continue;
                        } break; 
                    }
                } break;
                case MONSTER:{ // Entity A is a monster
                    switch(Bgroup){
                        case MONSTER:
                        case LOOTBAGGROUP:
                        case PORTAL:
                        case PLAYER: {
                            continue;
                        } break;
                    }
                } break;
                case PROJECTILE:{ // Entity A is a projectile
                    switch(Bgroup){
                        case LOOTBAGGROUP:
                        case PORTAL: 
                        case PROJECTILE: {
                            continue;
                        } break;
                    }
                } break;
                case WALLBOX:{ // Entity A is a wall
                    switch(Bgroup){
                        case PORTAL:
                        case WALLBOX:
                        case LOOTBAGGROUP: {
                            continue;
                        } break;
                    }
                } break;
                case LOOTBAGGROUP:{ // Entity A is a loot bag
                    switch(Bgroup){
                        case MONSTER:
                        case PROJECTILE:
                        case WALLBOX:
                        case LOOTBAGGROUP:
                        case PORTAL:{
                            continue;
                        } break;
                    }
                } break;
                case PORTAL:{ // Entity A is a portal
                    switch(Bgroup){
                        case MONSTER:
                        case PROJECTILE:
                        case WALLBOX:
                        case LOOTBAGGROUP:
                        case PORTAL:{
                            continue;
                        } break;
                    }
                } break;
            }

            // at this point, if one is a projectile, other one guaranteed to be an actionable entity (monster/player)
            // thus, there is no need to verify other entity is a player or monster
            bool aIsProjectile = (Agroup == PROJECTILE);
            bool bIsProjectile = (Bgroup == PROJECTILE);
            auto& bTransform = b.GetComponent<TransformComponent>();
            auto& bCollider = b.GetComponent<BoxColliderComponent>();
            if(!CheckAABBCollision( // if b projectile, increase a hitbox and vice-versa
                aTransform.position.x + aCollider.offset[0],
                aTransform.position.y + aCollider.offset[1] - (bIsProjectile *  30),
                aCollider.width,
                aCollider.height + (bIsProjectile * 30),
                bTransform.position.x + bCollider.offset[0],
                bTransform.position.y + bCollider.offset[1] - (aIsProjectile * 30),
                bCollider.width,
                bCollider.height + (aIsProjectile * 30))){
                    continue; // if no collision, continue
            }

            // switch case for actionable cases 
            switch(Agroup){
                case PLAYER:{ // Entity A is player
                    switch(Bgroup){ 
                        case PORTAL:{
                            auto& playerIC = a.GetComponent<PlayerItemsComponent>();
                            if(!playerIC.viewingBag && !playerIC.viewingPortal){
                                const auto& area = b.GetComponent<PortalComponent>().area;
                                eventBus->EmitEvent<PortalCollisionEvent>(true, a, b, registry);
                            }
                        } break;
                        case LOOTBAGGROUP:{
                            auto& playerIC = a.GetComponent<PlayerItemsComponent>();
                            if(!playerIC.viewingBag){
                                if(playerIC.viewingPortal){
                                    eventBus->EmitEvent<PortalCollisionEvent>(false, a, b, registry); // if player is viewing portal, stop viewing portal; give lootbag priority
                                }
                                if(!b.GetComponent<LootBagComponent>().opened){ // need not open an open bag
                                    eventBus->EmitEvent<LootBagCollisionEvent>(b, 11, true, registry, a, eventBus);
                                }
                            }
                        } break;
                        case PROJECTILE:{
                            if(b.GetComponent<ProjectileComponent>().parentGroupEnumInt == 0){ // 0 = monster shot this projectile
                                eventBus->EmitEvent<ProjectileDamageEvent>(b,a, eventBus, registry, assetStore, factory, Setup, deadPlayer, activeCharacterID, characterManager); // 1st is projectile 
                                if(deadPlayer.level > 0){ // player died, we just ran Setup, so we must exit this update (entities vector is outdated)
                                    deadPlayer.level = -1;
                                    return;
                                } 
                            }
                        } break;
                        case WALLBOX:{
                            eventBus->EmitEvent<CollisionEvent>(b,a,getCollisionSide(aTransform.position.x + aCollider.offset[0],aTransform.position.y + aCollider.offset[1],aCollider.width,aCollider.height,bTransform.position.x + bCollider.offset[0],bTransform.position.y + bCollider.offset[1],bCollider.width,bCollider.height));
                        } break;
                    }

                } break;
                case MONSTER:{ // Entity A is a monster
                    switch(Bgroup){
                        case PROJECTILE:{ // b is projectile
                            if(b.GetComponent<ProjectileComponent>().parentGroupEnumInt == 4){ // 4 = player shot this projectile
                                eventBus->EmitEvent<ProjectileDamageEvent>(b,a, eventBus, registry, assetStore, factory, Setup, deadPlayer, activeCharacterID, characterManager); // 1st is projectile 
                            }
                        } break;
                        case WALLBOX:{ // b is wall
                            eventBus->EmitEvent<CollisionEvent>(b,a,getCollisionSide(aTransform.position.x + aCollider.offset[0],aTransform.position.y + aCollider.offset[1],aCollider.width,aCollider.height,bTransform.position.x + bCollider.offset[0],bTransform.position.y + bCollider.offset[1],bCollider.width,bCollider.height));
                        } break;
                    }
                } break;
                case PROJECTILE:{ // Entity A is a projectile
                    switch(Bgroup){
                        case PLAYER:{
                            if(a.GetComponent<ProjectileComponent>().parentGroupEnumInt == 0){ // 0 = monster shot this projectile
                                eventBus->EmitEvent<ProjectileDamageEvent>(a,b, eventBus, registry, assetStore, factory, Setup, deadPlayer, activeCharacterID, characterManager); // 1st is projectile
                                if(deadPlayer.level > 0){ // player died, we just ran Setup, so we must exit this update (entities vector is outdated)
                                    deadPlayer.level = -1;
                                    return;
                                } 
                            }
                        } break;
                        case MONSTER:{
                            if(a.GetComponent<ProjectileComponent>().parentGroupEnumInt == 4){ // 4 = monster shot this projectile
                                eventBus->EmitEvent<ProjectileDamageEvent>(a,b, eventBus, registry, assetStore, factory, Setup, deadPlayer, activeCharacterID, characterManager); // 1st is projectile 
                            }
                        } break;
                        case WALLBOX:{
                            const auto& projectileBirthTime = a.GetComponent<ProjectileComponent>().startTime;
                            if(SDL_GetTicks() >= projectileBirthTime + 75){ // projectile immune from wall collision for first Xms
                                a.Kill();
                            }
                        } break;
                    }
                } break;
                case WALLBOX:{ // Entity A is a wall
                    switch(Bgroup){
                        case PLAYER:
                        case MONSTER:{
                            eventBus->EmitEvent<CollisionEvent>(a,b,getCollisionSide(bTransform.position.x + bCollider.offset[0],bTransform.position.y + bCollider.offset[1],bCollider.width,bCollider.height,aTransform.position.x + aCollider.offset[0],aTransform.position.y + aCollider.offset[1],aCollider.width,aCollider.height));
                        } break;
                        case PROJECTILE:{
                            const auto& projectileBirthTime = b.GetComponent<ProjectileComponent>().startTime;
                            if(SDL_GetTicks() >= projectileBirthTime + 75){ // projectile immune from wall collision for first Xms
                                b.Kill();
                            }
                        } break;
                    }
                } break;
                case LOOTBAGGROUP:{ // Entity A is a loot bag
                    switch(Bgroup){
                        case PLAYER:{
                            auto& playerIC = b.GetComponent<PlayerItemsComponent>();
                            if(!playerIC.viewingBag){
                                if(playerIC.viewingPortal){
                                    eventBus->EmitEvent<PortalCollisionEvent>(false, b, a, registry); // if player is viewing portal, stop viewing portal; give lootbag priority
                                }
                                if(!a.GetComponent<LootBagComponent>().opened){ // need not open an open bag
                                    eventBus->EmitEvent<LootBagCollisionEvent>(a, 11, true, registry, b, eventBus);
                                }                            
                            }
                        } break;
                    }
                } break;
                case PORTAL:{ // Entity A is a portal
                    switch(Bgroup){
                        case PLAYER:{
                            auto& playerIC = b.GetComponent<PlayerItemsComponent>();
                            if(!playerIC.viewingBag && !playerIC.viewingPortal){
                                const auto& area = a.GetComponent<PortalComponent>().area;
                                eventBus->EmitEvent<PortalCollisionEvent>(true, b, a, registry);
                            }
                        } break;
                    }
                } break;

            }

        }
    }
}