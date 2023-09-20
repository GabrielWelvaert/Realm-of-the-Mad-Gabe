#pragma once

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

/*
This system is responsible for calculating damage and emitting events such as level up and death
It is also resposible for increasing hp based off of vitality and deltatime
*/


class DamageSystem: public System{
    private:
        Xoshiro256 RNG; // fast RNG for calculating damange in random range ex: RNG.damange(min, max)

        // this hash table stops piercing projectiles from hitting same entity more than once
        // hash table messes with cache hits but idrk if this one can be removed unless I can think of a new solution
        // not too dire since its 1. being used only for piercing weapons (just player) 2. during events, not updates 
        std::unordered_map<unsigned int, std::unordered_set<unsigned int>> projectileVictimsAsCIDs; //projectile victim creationIDs

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

        inline void displayXPText(ProjectileDamageEvent& event, const glm::vec2& playerPosition, const int& xp){
            Entity dmgText = event.registry->CreateEntity();
            // std::cout << "victimposition in displayerdamagetext func " << victimPosition.x << ", " << victimPosition.y << std::endl;
            dmgText.AddComponent<TextLabelComponent>(
                "+" + std::to_string(xp) + "XP",
                "damagefont",
                xpgreen,
                false,
                350,
                0,
                1
                );
            dmgText.AddComponent<TransformComponent>(playerPosition);
        }

    public:
        DamageSystem(){
            RequireComponent<BoxColliderComponent>();
            RequireComponent<HPMPComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<ProjectileDamageEvent>(this, &DamageSystem::onProjectileCollision);
        }

        void onProjectileCollision(ProjectileDamageEvent& event){
            auto& projectileComponent = event.projectile.GetComponent<ProjectileComponent>();
            auto& victimHPMPComponent = event.victim.GetComponent<HPMPComponent>();
            const auto& victimPosition = event.victim.GetComponent<TransformComponent>().position;
            soundEnums hitSoundId;
            soundEnums deathSoundId;
            
            if(projectileComponent.damage == -1){return;} // flag 

            // defense calculation
            unsigned short realdamage;
            if(projectileComponent.damage >= victimHPMPComponent.activedefense){
                realdamage = projectileComponent.damage - victimHPMPComponent.activedefense;
            } else {
                realdamage = 0; // set to 0 instead of overflowing in cases where dmg < def 
            }

            // in rotmg, minimum damage reduction from defense is 90%
            if(realdamage < projectileComponent.damage * .1){
                realdamage = projectileComponent.damage * .1;
            }

            if(event.victim.GetId() == 0){
                soundEnums playerHitSounds[6] = {ARCHERHIT,KNIGHTHIT,PALADINHIT,PRIESTHIT,ROGUEHIT,WARRIORHIT};
                int noise = RNG.randomFromRange(0,5);
                hitSoundId = playerHitSounds[noise];
                //TODO if jugg exists: armored calculation lol
            } else {
                hitSoundId = victimHPMPComponent.hitsound;
                deathSoundId = victimHPMPComponent.deathsound;
            }

            // piercing logic, inflict damage, play hit noise
            if(projectileComponent.piercing){
                if(projectileVictimsAsCIDs[event.projectile.GetCreationId()].find(event.victim.GetCreationId()) == projectileVictimsAsCIDs[event.projectile.GetCreationId()].end()){
                    victimHPMPComponent.activehp -= realdamage;
                    projectileVictimsAsCIDs[event.projectile.GetCreationId()].emplace(event.victim.GetCreationId());
                    if(victimHPMPComponent.activehp >= 0){ // victim hit but not dead (player or monster)
                        event.assetStore->PlaySound(hitSoundId);
                        if(realdamage > 0){
                                displayDamgeText(event,victimPosition,realdamage);
                        }
                    } else if(event.victim.BelongsToGroup(MONSTER)){ // victim is monster, and has < 0 hp; thus inflicter is player
                        event.victim.Kill(); 
                        event.assetStore->PlaySound(deathSoundId);  
                        const auto& projectileParent = projectileComponent.parent;
                        auto& playerBaseStats = projectileParent.GetComponent<BaseStatComponent>();
                        int xp = victimHPMPComponent.maxhp / 10;
                        if(xp > 0){ 
                            playerBaseStats.xp += xp;  
                            displayXPText(event, projectileParent.GetComponent<TransformComponent>().position , xp);
                            if(playerBaseStats.level < 20 && playerBaseStats.xp >= nextXPToLevelUp[playerBaseStats.level]){
                                while(playerBaseStats.xp >= nextXPToLevelUp[playerBaseStats.level] && playerBaseStats.level < 20){
                                    event.eventBus->EmitEvent<LevelUpEvent>(projectileParent, event.registry, event.eventBus);
                                }
                                event.assetStore->PlaySound(LEVELUP);
                            }
                        }
                    } // else if player is dead.. todo... 
                } 

            } else { // projectile doesnt pierce; destroy projectile
                victimHPMPComponent.activehp -= realdamage;
                //std::cout << "Entity " << event.victim.GetId() << " took " << realdamage << " damage from " << event.projectile.GetId() << " and now has " << victimHPMPComponent.activehp << " hp" << std::endl;
                if(victimHPMPComponent.activehp >= 0){
                    event.assetStore->PlaySound(hitSoundId);
                    if(realdamage > 0){
                        displayDamgeText(event,victimPosition,realdamage);
                    }
                } else if(event.victim.BelongsToGroup(MONSTER)){ // victim is monster, and has < 0 hp; thus inflicter is player
                    event.victim.Kill(); 
                    event.assetStore->PlaySound(deathSoundId);  
                    const auto& projectileParent = projectileComponent.parent;
                    auto& playerBaseStats = projectileParent.GetComponent<BaseStatComponent>();
                    int xp = victimHPMPComponent.maxhp / 10;
                    if(xp > 0){ 
                        playerBaseStats.xp += xp;  
                        displayXPText(event, projectileParent.GetComponent<TransformComponent>().position , xp);
                        if(playerBaseStats.level < 20 && playerBaseStats.xp >= nextXPToLevelUp[playerBaseStats.level]){
                            while(playerBaseStats.xp >= nextXPToLevelUp[playerBaseStats.level] && playerBaseStats.level < 20){
                                event.eventBus->EmitEvent<LevelUpEvent>(projectileParent, event.registry, event.eventBus);
                            }
                            event.assetStore->PlaySound(LEVELUP);
                        }
                    }
                } // else if player is dead.. todo... 

                projectileComponent.damage = -1; // pixel-perfect AABB will collide with all entities in-place; use -1 as flag to break next collision for this projectile
                event.projectile.Kill();
                
            }

        }

        void Update(float deltaTime){
            for(auto entity: GetSystemEntities()){
                auto& stats = entity.GetComponent<HPMPComponent>();
                auto& activehp = stats.activehp;
                const auto& maxhp = stats.maxhp;
                const auto& activevitality = stats.activevitality;

                if((activehp < maxhp) && (activehp > 0)){
                    activehp += ((1 + .24 * activevitality)/1000) * deltaTime;
                    //std::cout << "vitality healing entity " << entity.GetId() << " by " << ((1 + .24 * activevitality)/1000) * deltaTime << std::endl;
                    if(activehp > maxhp){
                        activehp = maxhp;
                    }
                }
                // monsters don't really use magic-points; just update player's mp if needed
                if(entity.GetId() == 0){
                    auto& activemp = stats.activemp;
                    const auto& maxmp = stats.maxmp;
                    const auto& activewisdom = stats.activewisdom;
                    if((activemp < maxmp) && (activemp > 0)){
                       activemp += .12 * (activewisdom + 8.3) * deltaTime/1000;
                        if(activemp > maxmp){
                            activemp = maxmp;
                        }
                    }
                }
            }
        }

};
