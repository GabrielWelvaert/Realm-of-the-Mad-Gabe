#include "DamageSystem.h"

DamageSystem::DamageSystem(){
    RequireComponent<BoxColliderComponent>();
    RequireComponent<HPMPComponent>();
    RequireComponent<StatusEffectComponent>();
}

void DamageSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<ProjectileDamageEvent>(this, &DamageSystem::onProjectileCollision);
}

void DamageSystem::onProjectileCollision(ProjectileDamageEvent& event){
    auto& projectileComponent = event.projectile.GetComponent<ProjectileComponent>();
    auto& victimHPMPComponent = event.victim.GetComponent<HPMPComponent>();
    const auto& victimPosition = event.victim.GetComponent<TransformComponent>().position;
    const auto& invulnerable = event.victim.GetComponent<StatusEffectComponent>().effects[INVULNERABLE];
    soundEnums hitSoundId,deathSoundId;
    
    if(projectileComponent.damage == -1){return;} // flag 

    // defense calculation
    unsigned short realdamage = projectileComponent.damage;
    
    if(invulnerable){
        realdamage = 0; // only case where 0 damage is permitted is when entity is invulnerable
    } else if(!projectileComponent.ignoresDefense){
        if(projectileComponent.damage >= victimHPMPComponent.activedefense){
            realdamage = projectileComponent.damage - victimHPMPComponent.activedefense;
        } else {
            realdamage = 0; // set to 0 instead of overflowing in cases where dmg < def 
        }
        // in rotmg, minimum damage reduction from defense is 90%
        if(realdamage < projectileComponent.damage * .1){
            realdamage = projectileComponent.damage * .1;
            if(realdamage < 1){
                realdamage = 1;
            }
        }
    }

    if(event.victim.BelongsToGroup(PLAYER)){
        hitSoundId = playerHitSounds[RNG.randomFromRange(0,5)];
    } else {
        hitSoundId = victimHPMPComponent.hitsound;
        deathSoundId = victimHPMPComponent.deathsound;
    }

    /*
    Just want to go on record and say the following code has duplicated logic.
    This code was developed alongside changing requirements. Thats my excuse, I guess. I could clean it up, but it wouldn't really improve performance at all.
    In the beginning, separating the logic by piercing didn't create as much duplicate logic, but over time, it became this... whatever
    */

    // piercing logic, inflict damage, play hit noise
    if(projectileComponent.piercing){
        if(projectileVictimsAsCIDs[event.projectile.GetCreationId()].find(event.victim.GetCreationId()) == projectileVictimsAsCIDs[event.projectile.GetCreationId()].end()){
            victimHPMPComponent.activehp -= realdamage;
            if(projectileComponent.inflictsStatusEffect){
                event.eventBus->EmitEvent<StatusEffectEvent>(event.victim, projectileComponent.statsusEffect, event.eventBus, event.registry, projectileComponent.SEdurationMS);
            }
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
                    displayXPText(event, projectileParent.GetComponent<TransformComponent>().position , xp, projectileComponent.parent);
                    if(playerBaseStats.level < 20 && playerBaseStats.xp >= nextXPToLevelUp[playerBaseStats.level]){
                        while(playerBaseStats.xp >= nextXPToLevelUp[playerBaseStats.level] && playerBaseStats.level < 20){
                            event.eventBus->EmitEvent<LevelUpEvent>(projectileParent, event.registry, event.eventBus);
                        }
                        event.assetStore->PlaySound(LEVELUP);
                    }
                }
                if(event.victim.HasComponent<ItemTableComponent>()){
                    auto& itc = event.victim.GetComponent<ItemTableComponent>();
                    if(!itc.hasAlreadySpawnedBag){
                        event.factory->createLootAtDeath(event.victim, event.registry, event.assetStore);        
                        itc.hasAlreadySpawnedBag = true;
                    }
                }
            } else if(event.victim.BelongsToGroup(PLAYER)){ // player dead
                event.dp.className = event.victim.GetComponent<ClassNameComponent>().classname;
                event.dp.level = event.victim.GetComponent<BaseStatComponent>().level;
                event.dp.xp = event.victim.GetComponent<BaseStatComponent>().xp;
                event.dp.murderer = projectileComponent.spriteOfParent;
                event.characterManager->KillCharacter(event.activeCharacterID);
                event.registry->killAllEntities();
                event.assetStore->PlaySound(DEATH);
                event.Setup(false, true, NEXUS);
                return;
            }
        } 

    } else { // projectile doesnt pierce; destroy projectile
        victimHPMPComponent.activehp -= realdamage;
        if(projectileComponent.inflictsStatusEffect){
            event.eventBus->EmitEvent<StatusEffectEvent>(event.victim, projectileComponent.statsusEffect, event.eventBus, event.registry, projectileComponent.SEdurationMS);
        }
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
                displayXPText(event, projectileParent.GetComponent<TransformComponent>().position , xp, projectileComponent.parent);
                if(playerBaseStats.level < 20 && playerBaseStats.xp >= nextXPToLevelUp[playerBaseStats.level]){
                    while(playerBaseStats.xp >= nextXPToLevelUp[playerBaseStats.level] && playerBaseStats.level < 20){
                        event.eventBus->EmitEvent<LevelUpEvent>(projectileParent, event.registry, event.eventBus);
                    }
                    event.assetStore->PlaySound(LEVELUP);
                }
            }
            if(event.victim.HasComponent<ItemTableComponent>()){
                auto& itc = event.victim.GetComponent<ItemTableComponent>();
                if(!itc.hasAlreadySpawnedBag){
                    event.factory->createLootAtDeath(event.victim, event.registry, event.assetStore);        
                    itc.hasAlreadySpawnedBag = true;
                }
            }
        } else if(event.victim.BelongsToGroup(PLAYER)){ // player dead
            event.dp.className = event.victim.GetComponent<ClassNameComponent>().classname;
            event.dp.level = event.victim.GetComponent<BaseStatComponent>().level;
            event.dp.xp = event.victim.GetComponent<BaseStatComponent>().xp;
            event.dp.murderer = projectileComponent.spriteOfParent;
            event.characterManager->KillCharacter(event.activeCharacterID);
            event.registry->killAllEntities();
            event.assetStore->PlaySound(DEATH);
            event.Setup(false, true, NEXUS);
            return;
        } 

        projectileComponent.damage = -1; // pixel-perfect AABB will collide with all entities in-place; use -1 as flag to break next collision for this projectile
        event.projectile.Kill();
        
    }

}


// wisom and vitality have beeen buffed exponentially; in the original game, their restoration was unnoticeable until max level
void DamageSystem::Update(double deltaTime, Entity player){
    // wis regen mp for player. monsters dont have mana
    auto& hpmp = player.GetComponent<HPMPComponent>();
    const auto& quiet = player.GetComponent<StatusEffectComponent>().effects[QUIET];
    if(!quiet && hpmp.activemp < hpmp.maxmp){
        hpmp.activemp += .12 * (hpmp.activewisdom + 8.3) * deltaTime/150;
        if(hpmp.activemp > hpmp.maxmp){
            hpmp.activemp = hpmp.maxmp;
        }
    } 

    // vit regen hp
    for(auto& entity: GetSystemEntities()){
        auto& stats = entity.GetComponent<HPMPComponent>();
        auto& activehp = stats.activehp;
        const auto& maxhp = stats.maxhp;
        const auto& activevitality = stats.activevitality;

        if((activehp < maxhp) && (activehp > 0)){
            activehp += ((1 + .24 * activevitality)/250) * deltaTime;
            if(activehp > maxhp){
                activehp = maxhp;
            }
        }
    }
}