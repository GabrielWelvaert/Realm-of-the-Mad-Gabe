#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/CollisionFlagComponent.h"
#include "../Components/StatusEffectComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include <unordered_map>
#include "../../libs/SDL2/SDL.h"

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

inline bool CheckAABBCollision(double ax, double ay, double aw, double ah, double bx, double by, double bw, double bh){
    return (
        ax < bx + bw &&
        ax + aw > bx &&
        ay < by + bh &&
        ay + ah > by
    );
}

/*
This system is responsible for updating postion based off of velocity, deltatime, and flags. It also containts logic for collisionEvents. Projectiles do not move in this system!
*/

class MovementSystem: public System {
    public:
        MovementSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<RidigBodyComponent>();
            RequireComponent<CollisionFlagComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::onCollision);
        }

        // wall collision event, emitted by movementSystem!
        void onCollision(CollisionEvent& event){
            if(!(event.b.BelongsToGroup(MONSTER) || event.b.GetId() == 0)){
                std::cout << "wall being used as monster/player in movementSystem::onCollision!" << std::endl;
            } 
            auto& transform = event.b.GetComponent<TransformComponent>();
            auto& flags = event.b.GetComponent<CollisionFlagComponent>();
            const auto& activespeed = event.b.GetComponent<SpeedStatComponent>().activespeed;
            auto& lastSideHitFlag = flags.lastSideHitFlag;
            auto& lastlastSideHitFlag = flags.lastlastSideHitflag;
            auto& msLastCollisionFlag = flags.msLastCollisionFlag;
            int move = .0167 * activespeed + 2.495;
            bool firstmove = false;
            int msDoubleCollisionBuffer = 10; // may need to be scaled with deltatime / speed
            sidehit currentSide = event.side;

            if(msLastCollisionFlag == 0){ //firstmove case
                msLastCollisionFlag = SDL_GetTicks();
                flags.lastSideHitFlag = currentSide;
                firstmove = true;
                flags.idOfWallHit = 0;
            }

            if(!(SDL_GetTicks() - msLastCollisionFlag < msDoubleCollisionBuffer && !firstmove && lastSideHitFlag == currentSide)){ //if clause detects double collision as to stop "double" bumping
                msLastCollisionFlag = SDL_GetTicks();
                lastlastSideHitFlag = lastSideHitFlag;
                lastSideHitFlag = currentSide;
                if(event.side == RIGHTSIDE){ //right
                    transform.position.x = std::round(transform.position.x += move  ); 
                    flags.collisionFlag = RIGHTSIDE; //right
                }
                else if (event.side == LEFTSIDE){ //left 
                    transform.position.x = std::round(transform.position.x -= move  );
                    flags.collisionFlag = LEFTSIDE; //left
                } 
                else if (event.side == TOPSIDE){ // top
                    transform.position.y = std::round(transform.position.y -= move  );
                    flags.collisionFlag = TOPSIDE; //up
                } 
                else {      // down
                    transform.position.y = std::round(transform.position.y += move  );
                    flags.collisionFlag = BOTTOMSIDE; //down
                }

                // storing Id of wall to later be used to check for re-collision with same wall in update)
                flags.idOfOldWallHit = flags.idOfWallHit;
                flags.idOfWallHit = event.a.GetId();
                // std::cout << event.a.GetId() << " stored as last hit wall " << std::endl;
            }

        }   

        void Update(const double& deltaTime, std::unique_ptr<Registry>& registry) {
            for (auto entity: GetSystemEntities()){
                if(entity.HasComponent<StatusEffectComponent>()){
                    if(entity.GetComponent<StatusEffectComponent>().effects[PARALYZE]){
                        return; // entity is paralyzed; do not move! 
                    }
                }

                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rigidbody = entity.GetComponent<RidigBodyComponent>();
                auto& flags = entity.GetComponent<CollisionFlagComponent>();
                float speedinpixelspersecond;

                if(entity.HasComponent<SpeedStatComponent>()){ // things like projectiles dont have statComponent
                    const auto& activespeed = entity.GetComponent<SpeedStatComponent>().activespeed;
                    speedinpixelspersecond = 2.25 * activespeed + 120; // = (0.0746667 * SPD + 3.9813333) * 35
                    
                    
                    rigidbody.velocity.x *= speedinpixelspersecond;
                    rigidbody.velocity.y *= speedinpixelspersecond;
                }

                

                // if collionFlag, check if next move will result in collision and release if necessary
                if(flags.collisionFlag != NONESIDE){
                    auto aTransform = transform;
                    const auto& aCollider = entity.GetComponent<BoxColliderComponent>();
                    const auto& bTransform = registry->GetComponent<TransformComponent>(flags.idOfWallHit);
                    const auto& bCollider = registry->GetComponent<BoxColliderComponent>(flags.idOfWallHit);
                    const auto& bflags = registry->GetComponent<CollisionFlagComponent>(flags.idOfWallHit);
                    const auto& cTransform = registry->GetComponent<TransformComponent>(flags.idOfOldWallHit);
                    const auto& cCollider = registry->GetComponent<BoxColliderComponent>(flags.idOfOldWallHit);
                    const auto& cflags = registry->GetComponent<CollisionFlagComponent>(flags.idOfOldWallHit);

                    aTransform.position.x += rigidbody.velocity.x * deltaTime;
                    aTransform.position.y += rigidbody.velocity.y * deltaTime;
                    bool nextMoveWillHitSameWall = CheckAABBCollision(
                        aTransform.position.x + aCollider.offset[0],
                        aTransform.position.y + aCollider.offset[1],
                        aCollider.width,
                        aCollider.height,
                        bTransform.position.x + bCollider.offset[0],
                        bTransform.position.y + bCollider.offset[1],
                        bCollider.width,
                        bCollider.height);

                    if(!nextMoveWillHitSameWall){ // will not hit same wall; release flag from double collision
                        // std::cout << "not gonna hit same wall " << std::endl;
                        flags.collisionFlag = NONESIDE;
                    }
                    if(flags.idOfOldWallHit != flags.idOfWallHit && flags.idOfOldWallHit != 0){ // check for corner collision, if re-hit, apply respective flag
                        bool nextMoveWIllCauseCornerCollision = CheckAABBCollision(
                        aTransform.position.x + aCollider.offset[0],
                        aTransform.position.y + aCollider.offset[1],
                        aCollider.width,
                        aCollider.height,
                        cTransform.position.x + cCollider.offset[0],
                        cTransform.position.y + cCollider.offset[1],
                        cCollider.width,
                        cCollider.height);
                        if(nextMoveWIllCauseCornerCollision){ // this corner collision stuff stops phasing thru walls at corners! 
                            flags.collisionFlag = getCollisionSide(aTransform.position.x + aCollider.offset[0],aTransform.position.y + aCollider.offset[1],aCollider.width,aCollider.height,cTransform.position.x + cCollider.offset[0],cTransform.position.y + cCollider.offset[1],cCollider.width,cCollider.height); // wall passed as 2nd in getCollisionSide
                            if((flags.lastSideHitFlag == TOPSIDE && flags.lastlastSideHitflag == LEFTSIDE) || (flags.lastSideHitFlag == LEFTSIDE && flags.lastlastSideHitflag == TOPSIDE)){
                                transform.position.y -= 1;
                                transform.position.x -= 1;
                                // std::cout << "tl" << std::endl;
                                // if(rigidbody.velocity.x > 0){rigidbody.velocity.x = 0;} // hit left, so dont move right
                                // if(rigidbody.velocity.y > 0){rigidbody.velocity.y = 0;} // hit top, so dont move down
                            } else if (((flags.lastSideHitFlag == TOPSIDE && flags.lastlastSideHitflag == RIGHTSIDE) || (flags.lastSideHitFlag == RIGHTSIDE && flags.lastlastSideHitflag == TOPSIDE))) {
                                transform.position.y -= 1;
                                transform.position.x += 1;
                                // std::cout << "tr" << std::endl;
                                // if(rigidbody.velocity.x < 0){rigidbody.velocity.x = 0;} // hit right, so dont move left
                                // if(rigidbody.velocity.y > 0){rigidbody.velocity.y = 0;} // hit top, so dont move down
                            } else if (((flags.lastSideHitFlag == BOTTOMSIDE && flags.lastlastSideHitflag == LEFTSIDE) || (flags.lastSideHitFlag == LEFTSIDE && flags.lastlastSideHitflag == BOTTOMSIDE))) {
                                transform.position.y += 1;
                                transform.position.x -= 1;
                                // std::cout << "bl" << std::endl;
                                // if(rigidbody.velocity.x > 0){rigidbody.velocity.x = 0;} // hit left, so dont move right
                                // if(rigidbody.velocity.y < 0){rigidbody.velocity.y = 0;} // hit bottom, so dont move up
                            } else {
                                transform.position.y += 1; //BR
                                transform.position.x += 1;
                                // std::cout << "br" << std::endl;
                                // if(rigidbody.velocity.x < 0){rigidbody.velocity.x = 0;} // hit right, so dont move left
                                // if(rigidbody.velocity.y < 0){rigidbody.velocity.y = 0;} // hit bottom, so dont move up
                            }
                            
                            
                        }
                    
                    }
                }


                if (flags.collisionFlag == RIGHTSIDE && rigidbody.velocity.x < 0){ // moving left
                    rigidbody.velocity.x = 0.0;
                } else if (flags.collisionFlag == LEFTSIDE && rigidbody.velocity.x > 0){ // moving right
                    rigidbody.velocity.x = 0.0;
                } else if (flags.collisionFlag == TOPSIDE && rigidbody.velocity.y > 0){ // moving down
                    rigidbody.velocity.y = 0.0;
                } else if (flags.collisionFlag == BOTTOMSIDE && rigidbody.velocity.y < 0) { // moving up
                    rigidbody.velocity.y = 0.0;
                } 
                
                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;

                // transform.collisionFlag = NONESIDE;
            }
        }



};

#endif
