#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/ProjectileDamageEvent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/StatusEffectComponent.h"
#include <chrono>
#include <cstdint> 
#include <unistd.h>
#include "../Utils/enums.h"
#include "../Utils/tables.h"
#include "../Utils/Xoshiro256.h"
#include "../Events/LevelUpEvent.h"
#include <unordered_map>
#include <unordered_set>
#include "../Utils/colors.h"
#include "../Components/ItemTableComponent.h"
#include "../Events/StatusEffectEvent.h"

/*
This system is responsible for calculating damage and emitting events such as level up and death
It is also resposible for increasing hp based off of vitality and deltatime
*/


class DamageSystem: public System{
    private:
        Xoshiro256 RNG; // fast RNG for calculating damange in random range ex: RNG.damange(min, max)

        std::unordered_map<unsigned int, std::unordered_set<unsigned int>> projectileVictimsAsCIDs; //projectile victim creationIDs

        soundEnums playerHitSounds[6] = {ARCHERHIT,KNIGHTHIT,PALADINHIT,PRIESTHIT,ROGUEHIT,WARRIORHIT};

        inline void displayDamgeText(ProjectileDamageEvent& event, const glm::vec2& victimPosition, const int& dmg){
            Entity dmgText = event.registry->CreateEntity();
            // std::cout << "victimposition in displayerdamagetext func " << victimPosition.x << ", " << victimPosition.y << std::endl;
            dmgText.AddComponent<TextLabelComponent>(
                "-" + std::to_string(dmg),
                "damagefont",
                damagered,
                false,
                350,
                event.victim.GetId(),
                event.victim.GetCreationId()
                );
            dmgText.AddComponent<TransformComponent>(victimPosition);
        }

        inline void displayXPText(ProjectileDamageEvent& event, const glm::vec2& playerPosition, const int& xp, const Entity& player){
            Entity dmgText = event.registry->CreateEntity();
            // std::cout << "victimposition in displayerdamagetext func " << victimPosition.x << ", " << victimPosition.y << std::endl;
            dmgText.AddComponent<TextLabelComponent>(
                "+" + std::to_string(xp) + "XP",
                "damagefont",
                xpgreen,
                false,
                350,
                player.GetId(),
                player.GetCreationId()
                );
            dmgText.AddComponent<TransformComponent>(playerPosition);
        }

    public:
        DamageSystem();

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);

        void onProjectileCollision(ProjectileDamageEvent& event);

        void Update(double deltaTime, Entity player);

};

#endif