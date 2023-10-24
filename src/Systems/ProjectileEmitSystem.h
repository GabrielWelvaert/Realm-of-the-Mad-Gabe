#ifndef PROJECTILEEMITSYSTEM_H
#define PROJECTILEEMITSYSTEM_H

#include "../Components/ProjectileEmitterComponent.h"
#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include <unordered_map>
#include "../utils/enums.h"
#include "../utils/Xoshiro256.h"
#include "../Events/WeaponEquipEvent.h"
#include "../EventBus/EventBus.h"
#include "../AssetStore/AssetStore.h"
/*
This system is responsible for emitting projectiles when enough time has passed for successive shot(s) to be fired
It also contains the algorithm for shots with multiple projectiles
*/

class ProjectileEmitSystem: public System{
    private:
        Xoshiro256 RNG;

        const soundEnums playerSounds[12] = {BLADESWING, ARROWSHOOT, MAGICSHOOT, MAGICSHOOT, BLADESWING, BLADESWING, BLADESWING, BLADESWING, MAGICSHOOT, ARROWSHOOT, MAGICSHOOT, BLADESWING};
        const int projectilescale = 5;
        // given projectile origin and destination coordiantes and ProjectileEmitterComponent.velocity reference
        // 1) updates ProjectileEmitterComponent velocity 
        // 2) returns degrees of rotation to be added to projectile's transformComponent.rotation 
        // diagonal parameter is a flag to adjust the angle for sprites that start with +45 degrees
        // assumes projectile sprite is: ↑ and a diagonal projectile is ↗
        inline float getRotationFromCoordiante(const float& projectileSpeed, const float& originX, const float& originY, const float& destX, const float& destY, glm::vec2& emitterVelocity, const bool& diagonal = false){
            float angleRadians = std::atan2(destY - originY, destX - originX);   
            float angleDegrees = angleRadians * (180.0 / M_PI);
            emitterVelocity.x = projectileSpeed * std::cos(angleRadians);
            emitterVelocity.y = projectileSpeed * std::sin(angleRadians);
            return fmod(angleDegrees + 90.0, 360.0) - 45*diagonal; // fmod shit because degrees=0 is top right
        }

        // derives velocity for arc-gap-difference projectiles given the origin projectile velocity
        inline void projectileVelocityArcGap(const glm::vec2& originVelocity, const float& rotationDegrees, const float& deltaDegrees, glm::vec2& emitterVelocity){
            float deltaRadians = deltaDegrees * (M_PI / 180.0);
            emitterVelocity.x = originVelocity.x * std::cos(deltaRadians) - originVelocity.y * std::sin(deltaRadians);
            emitterVelocity.y = originVelocity.x * std::sin(deltaRadians) + originVelocity.y * std::cos(deltaRadians);
        }


    public:
        ProjectileEmitSystem();

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);
        void onPlayerEquippedWeapon(WeaponEquipEvent& event);

        void Update(std::unique_ptr<Registry>& registry, SDL_Rect camera, int mx, int my, glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore);
};


#endif