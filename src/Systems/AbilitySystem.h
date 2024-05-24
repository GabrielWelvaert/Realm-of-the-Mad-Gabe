#ifndef ABILITYSYSTEM_H
#define ABILITYSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/AbilityComponent.h"
#include "../Events/EquipAbilityEvent.h"
#include "../EventBus/EventBus.h"
#include "../Components/ClassNameComponent.h"
#include "../Utils/tables.h"
#include "../Events/TomeUseEvent.h"
#include "../Events/QuiverUseEvent.h"
#include "../Events/SpellUseEvent.h"
#include "../Events/CloakUseEvent.h"
#include "../Events/ShieldUseEvent.h"
#include "../Utils/Xoshiro256.h"
#include "../Events/HelmUseEvent.h"
#include "../Events/StatusEffectEvent.h"
#include "../Components/TextLabelComponent.h"
#include "../Utils/colors.h"
#include "../Components/TransformComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/LinearProjectileComponent.h"
#include "../Components/OscillatingProjectileComponent.h"
#include "../Components/RotationComponent.h"

/*
This system is responsible for being the event handler for equipping abilities and using them!
*/

class AbilitySystem: public System{
    private:

        Xoshiro256 RNG;

        std::vector<float> spellAngles = std::vector<float>(20);

        inline float getRotationFromCoordiante(const float& projectileSpeed, const float& originX, const float& originY, const float& destX, const float& destY, glm::vec2& emitterVelocity, const bool& diagonal = false){
            float angleRadians = std::atan2(destY - originY, destX - originX);   
            float angleDegrees = angleRadians * (180.0 / M_PI);
            emitterVelocity.x = projectileSpeed * std::cos(angleRadians);
            emitterVelocity.y = projectileSpeed * std::sin(angleRadians);
            return fmod(angleDegrees + 90.0, 360.0) - 45*diagonal; // fmod shit because degrees=0 is top right
        }

        inline void displayHealText(std::unique_ptr<Registry>& registry, const glm::vec2& playerPosition, const int& healAmount, const Entity& player){
            if(healAmount <= 0){return;}
            Entity dmgText = registry->CreateEntity();
            dmgText.AddComponent<TextLabelComponent>(
                "+" + std::to_string(healAmount),
                "damagefont",
                xpgreen,
                false,
                350,
                player.GetId(),
                player.GetCreationId()
                );
            dmgText.AddComponent<TransformComponent>(playerPosition);
        }
        
        // derives velocity for arc-gap-difference projectiles given the origin projectile velocity
        inline void projectileVelocityArcGap(const glm::vec2& originVelocity, const float& rotationDegrees, const float& deltaDegrees, glm::vec2& emitterVelocity){
            float deltaRadians = deltaDegrees * (M_PI / 180.0);
            emitterVelocity.x = originVelocity.x * std::cos(deltaRadians) - originVelocity.y * std::sin(deltaRadians);
            emitterVelocity.y = originVelocity.x * std::sin(deltaRadians) + originVelocity.y * std::cos(deltaRadians);
        }


    public:
        AbilitySystem();

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);

        void onTomeUse(TomeUseEvent& event);

        void onQuiverUse(QuiverUseEvent& event);
        void onHelmUse(HelmUseEvent& event);

        void onAbilityEquip(EquipAbilityEvent& event);
        void onSpellUse(SpellUseEvent& event);
        void onCloakUse(CloakUseEvent& event);
        void onShieldUse(ShieldUseEvent& event);
        


};

#endif