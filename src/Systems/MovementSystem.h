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
#include "../Components/OrbitalMovementComponent.h"
#include "../Components/ChaseAIComponent.h"
#include "../Components/AnimatedChaseAIComponent.h"

/*
This system is responsible for updating postion based off of velocity, deltatime, and flags. It also containts logic for collisionEvents. Projectiles do not move in this system!
*/

class MovementSystem: public System {
    private:
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


    public:
        MovementSystem();

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);

        // wall collision event, emitted by movementSystem!
        void onCollision(CollisionEvent& event);

        void Update(const double& deltaTime, std::unique_ptr<Registry>& registry, const glm::vec2& playerCenter);


};

#endif
