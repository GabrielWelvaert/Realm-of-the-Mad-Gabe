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
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"
#include "../Utils/factory.h"

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
        CollisionSystem();

        void Update(std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, const double& deltaTime, Entity& player, std::unique_ptr<Factory>& factory);
};

#endif
