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
#include "../Events/PortalCollisionEvent.h"
#include "../Utils/deadPlayer.h"
#include "../Events/AOEBombEvent.h"

/*
This system is responsible for parsing box-collider collision and emitting respective events
*/

class CollisionSystem: public System {
    private:
        inline bool CheckAABBCollision(double ax, double ay, double aw, double ah, double bx, double by, double bw, double bh) const {
            return (
                ax < bx + bw &&
                ax + aw > bx &&
                ay < by + bh &&
                ay + ah > by
            );
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

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);

        void onAOEBomb(AOEBombEvent& event);

        void Update(std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, const double& deltaTime, std::unique_ptr<Factory>& factory, const SDL_Rect& camera, std::function<void(bool, bool, wallTheme)> Setup, deadPlayer& deadPlayer, std::string& activeCharacterID, std::unique_ptr<CharacterManager>& characterManager);
};

#endif
