#pragma once 

#include "../ECS/ECS.h"
#include "../Components/AbilityComponent.h"
#include "../Events/EquipAbilityEvent.h"
#include "../EventBus/EventBus.h"
#include "../Components/ClassNameComponent.h"
#include "../Utils/tables.h"
#include "../Events/TomeUseEvent.h"
#include "../Events/QuiverUseEvent.h"
#include "../Utils/Xoshiro256.h"

/*
This system is responsible for being the event handler for equipping abilities and using them!
*/

class AbilitySytem: public System{
    private:

        Xoshiro256 RNG;

        inline float getRotationFromCoordiante(const float& projectileSpeed, const float& originX, const float& originY, const float& destX, const float& destY, glm::vec2& emitterVelocity, const bool& diagonal = false){
            float angleRadians = std::atan2(destY - originY, destX - originX);   
            float angleDegrees = angleRadians * (180.0 / M_PI);
            emitterVelocity.x = projectileSpeed * std::cos(angleRadians);
            emitterVelocity.y = projectileSpeed * std::sin(angleRadians);
            return fmod(angleDegrees + 90.0, 360.0) - 45*diagonal; // fmod shit because degrees=0 is top right
        }

    public:
        AbilitySytem(){
            RequireComponent<AbilityComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<EquipAbilityEvent>(this, &AbilitySytem::onAbilityEquip);
            eventBus->SubscribeToEvent<TomeUseEvent>(this, &AbilitySytem::onTomeUse);
            eventBus->SubscribeToEvent<QuiverUseEvent>(this, &AbilitySytem::onQuiverUse);
        }

        void onTomeUse(TomeUseEvent& event){
            // at this point we've done everything (check tome, mp, subtract mp) except use the tome, so use it!
            auto& HPMP = event.player.GetComponent<HPMPComponent>();
            const auto& tome = event.player.GetComponent<TomeComponent>();
            HPMP.activehp += tome.hp;
            if(HPMP.activehp > HPMP.maxhp){
                HPMP.activehp = HPMP.maxhp;
            }
        }

        void onQuiverUse(QuiverUseEvent& event){
            std::cout << "onQuiverUse" << std::endl;
            const auto& player = event.player;
            const auto& quiver = player.GetComponent<QuiverComponent>();
            const auto& playerpos = player.GetComponent<TransformComponent>().position;
            glm::vec2 originVelocity;
            float rotationDegrees = getRotationFromCoordiante(
                1024, 
                playerpos.x, 
                playerpos.y+10, 
                event.mx + event.camera.x, 
                event.my + event.camera.y,
                originVelocity,
                true);
            int damage = RNG.randomFromRange(quiver.minDamage, quiver.maxDamage);
            Entity projectile = event.registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(originVelocity);
            projectile.AddComponent<SpriteComponent>(quiver.texture, 8, 8, quiver.srcRect, 3, false, true);
            projectile.AddComponent<BoxColliderComponent>(8,8,glm::vec2(16,16));
            projectile.AddComponent<TransformComponent>(glm::vec2(playerpos.x, playerpos.y+10), glm::vec2(6.0,6.0), rotationDegrees);
            projectile.AddComponent<ProjectileComponent>(damage, 1000, true, player, 4);
            // todo add paralyze infliction to the projectile
            projectile.Group(PROJECTILE);
            
        }

        void onAbilityEquip(EquipAbilityEvent& event){
            std::cout << " abiltiy equip event handled! " << std::endl;
            auto& player = event.player;
            auto& classname = player.GetComponent<ClassNameComponent>().classname;
            auto& ac = player.GetComponent<AbilityComponent>();
            auto newAbilityData = itemEnumToAbilityData.at(event.itemEnum);
            ac.coolDownMS = newAbilityData.cooldown;
            ac.mpRequired = newAbilityData.mprequired;
            ac.abilityEquipped = true;

            switch(classname){
                case ARCHER:{
                    auto& quiver = player.GetComponent<QuiverComponent>();
                    const auto& newquiverdata = itemEnumToQuiverData.at(event.itemEnum);
                    quiver.maxDamage = newquiverdata.maxDamage;
                    quiver.minDamage = newquiverdata.minDamage;
                    quiver.srcRect = newquiverdata.srcRect;
                    quiver.texture = newquiverdata.texture;
                    break;
                }
                case PRIEST: {
                    auto& tome = player.GetComponent<TomeComponent>();
                    tome.hp = itemEnumToTomeData.at(event.itemEnum).hp;
                    break;
                }
                case WARRIOR: {
                    auto& helm = player.GetComponent<HelmComponent>();
                    break;
                }
            }

        }


};