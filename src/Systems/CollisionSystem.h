#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Events/ProjectileDamageEvent.h"
#include "../Components/ProjectileComponent.h"
#include "../Events/LootBagCollisionEvent.h"
#include "../Components/LootBagComponent.h"


/*
This system is responsible for parsing box-collider collision and emitting respective events
*/

class CollisionSystem: public System {
    private:
        inline bool CheckAABBCollision(double ax, double ay, double aw, double ah, double bx, double by, double bw, double bh){
            return (
                ax < bx + bw &&
                ax + aw > bx &&
                ay < by + bh &&
                ay + ah > by
            );
        }

        inline bool playerMonsterCollision(const Entity& a, const Entity& b){
            return a.BelongsToGroup(MONSTER) && b.BelongsToGroup(PLAYER) || a.BelongsToGroup(PLAYER) && b.BelongsToGroup(MONSTER);
        }

        inline bool oneEntityIsWall(const Entity& a, const Entity& b){
            return a.BelongsToGroup(WALLBOX) && !b.BelongsToGroup(WALLBOX) || !a.BelongsToGroup(WALLBOX) && b.BelongsToGroup(WALLBOX);
        }

        inline bool oneEntityIsProjectile(const Entity& a, const Entity& b){
            return a.BelongsToGroup(PROJECTILE) || b.BelongsToGroup(PROJECTILE);
        }

        inline bool projectileHitSomeone(const Entity& a, const Entity& b){
            return (a.BelongsToGroup(PROJECTILE) && (b.BelongsToGroup(PLAYER) || b.BelongsToGroup(MONSTER))) ||
                   (b.BelongsToGroup(PROJECTILE) && (a.BelongsToGroup(PLAYER) || a.BelongsToGroup(MONSTER)));        
        }

        inline bool bothAreWalls(const Entity& a, const Entity& b){
            return (a.BelongsToGroup(WALLBOX) && b.BelongsToGroup(WALLBOX)) ||
                   (b.BelongsToGroup(WALLBOX) && a.BelongsToGroup(WALLBOX));
        }

        inline bool twoMonsters(const Entity& a, const Entity & b){
            return a.BelongsToGroup(MONSTER) && b.BelongsToGroup(MONSTER);
        }

        inline bool projectileParentGroupSameAsVictimGroup(const Entity& a, const Entity & b){
            // at this point, we've deduced that projectile hit a monster or player
            if(a.BelongsToGroup(PROJECTILE)){ // a is the projectile
                if(b.BelongsToGroup(MONSTER)){ // victim is monster
                    return a.GetComponent<ProjectileComponent>().parentGroupEnumInt == 0;    
                } else { // victim must be player 
                    return a.GetComponent<ProjectileComponent>().parentGroupEnumInt == 4;
                }
            } else { // b is the projectile
                if(a.BelongsToGroup(MONSTER)){ // victim is monster
                    return b.GetComponent<ProjectileComponent>().parentGroupEnumInt == 0;
                } else { // victim must be player
                    return b.GetComponent<ProjectileComponent>().parentGroupEnumInt == 4;
                }
            }
        }

        inline bool bothAreProjectiles(const Entity& a, const Entity& b){
            return a.BelongsToGroup(PROJECTILE) && b.BelongsToGroup(PROJECTILE);
        }

        inline bool projectileAndVictim(const Entity& a, const Entity& b){
            return (a.BelongsToGroup(PROJECTILE) && (b.BelongsToGroup(PLAYER) || b.BelongsToGroup(MONSTER))) ||
                   (b.BelongsToGroup(PROJECTILE) && (a.BelongsToGroup(PLAYER) || a.BelongsToGroup(MONSTER)));
        }

        inline bool bagAndNotPlayer(const Entity& a, const Entity& b){
            return (a.BelongsToGroup(LOOTBAGGROUP) && !b.BelongsToGroup(PLAYER)) ||
                   (!a.BelongsToGroup(PLAYER) && b.BelongsToGroup(LOOTBAGGROUP));
            
        }

        inline bool playerAndBag(const Entity& a, const Entity& b){
            return (a.BelongsToGroup(PLAYER) && b.BelongsToGroup(LOOTBAGGROUP)) ||
                   (a.BelongsToGroup(LOOTBAGGROUP) && b.BelongsToGroup(PLAYER));
        }

        // if A is wall and B is player, will return side of wall collided with relative to player! 
        // if it returns RIGHTSIDE, you hit the rightside of the wall 
        inline sidehit getCollisionSide(double ax, double ay, double aw, double ah, double bx, double by, double bw, double bh) {
            double distanceX = (ax + aw / 2) - (bx + bw / 2);
            double distanceY = (ay + ah / 2) - (by + bh / 2);
            double extentX = aw / 2 + bw / 2;
            double extentY = ah / 2 + bh / 2;
            double overlapX = extentX - std::abs(distanceX);
            double overlapY = extentY - std::abs(distanceY);
            double minOverlap = std::min(overlapX, overlapY);

            if (overlapX == minOverlap) {
                if (distanceX > 0) {
                    return RIGHTSIDE; // right 
                } else {
                    return LEFTSIDE; // left 
                }
            } else {
                if (distanceY > 0) {
                    return BOTTOMSIDE; // down 
                } else {
                    return TOPSIDE; // up
                }
            }
        }

    public:
        CollisionSystem(){
            RequireComponent<BoxColliderComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, const double& deltaTime, PlayerItemsComponent& playerIC) {
            auto entities = GetSystemEntities();
            for (auto i = entities.begin(); i != entities.end(); i++){ //sliding window algorithm
                Entity a = *i;
                auto aTransform = a.GetComponent<TransformComponent>();
                auto aCollider = a.GetComponent<BoxColliderComponent>();
                //all entities to the right of i
                for (auto j = i; j != entities.end(); j++){
                
                    Entity b = *j;
                    if( a==b || playerMonsterCollision(a,b) || bothAreWalls(a,b) || twoMonsters(a,b) || bothAreProjectiles(a,b) || bagAndNotPlayer(a,b) ) {continue;} 
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
                            a.BelongsToGroup(PROJECTILE) ? eventBus->EmitEvent<ProjectileDamageEvent>(a,b, eventBus, registry, assetStore) : eventBus->EmitEvent<ProjectileDamageEvent>(b,a, eventBus, registry, assetStore); 
                        } else if (playerAndBag(a,b)){ // player collided with loot bag
                            if(!playerIC.ptrToOpenBag){ // only open new bag if not currently viewing a bag
                                if(a.BelongsToGroup(PLAYER)){ // b is the loot bag
                                    if(!b.GetComponent<LootBagComponent>().opened){ // need not open an open bag
                                        eventBus->EmitEvent<LootBagCollisionEvent>(b, 11, true, registry, playerIC);
                                    }
                                } else { // a is the loot bag
                                    if(!a.GetComponent<LootBagComponent>().opened){ // need not open an open bag
                                        eventBus->EmitEvent<LootBagCollisionEvent>(a, 11, true, registry, playerIC);
                                    }
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
};

#endif
