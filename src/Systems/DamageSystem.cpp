#include "DamageSystem.h"

DamageSystem::DamageSystem(){
    RequireComponent<BoxColliderComponent>();
    RequireComponent<HPMPComponent>();
}

void DamageSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<ProjectileDamageEvent>(this, &DamageSystem::onProjectileCollision);
}

void DamageSystem::onProjectileCollision(ProjectileDamageEvent& event){
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
        if(realdamage < 1){
            realdamage = 1;
        }
    }

    if(event.victim.BelongsToGroup(PLAYER)){
        soundEnums playerHitSounds[6] = {ARCHERHIT,KNIGHTHIT,PALADINHIT,PRIESTHIT,ROGUEHIT,WARRIORHIT};
        int noise = RNG.randomFromRange(0,5);
        hitSoundId = playerHitSounds[noise];
        //TODO if jugg exists: armored calculation lol
    } else {
        hitSoundId = victimHPMPComponent.hitsound;
        deathSoundId = victimHPMPComponent.deathsound;
    }

    /*
    Just want to go on record and say the following code has duplicated logic. dont ask me why I did it this way. 
    */

    // piercing logic, inflict damage, play hit noise
    if(projectileComponent.piercing){
        if(projectileVictimsAsCIDs[event.projectile.GetCreationId()].find(event.victim.GetCreationId()) == projectileVictimsAsCIDs[event.projectile.GetCreationId()].end()){
            victimHPMPComponent.activehp -= realdamage;
            if(projectileComponent.statsusEffect){
                if(event.victim.HasComponent<StatusEffectComponent>() && !event.victim.GetComponent<StatusEffectComponent>().effects[projectileComponent.statsusEffect]){
                    event.eventBus->EmitEvent<StatusEffectEvent>(event.victim, projectileComponent.statsusEffect, event.eventBus, event.registry, projectileComponent.SEdurationMS);    
                }
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
        if(projectileComponent.statsusEffect){
            if(event.victim.HasComponent<StatusEffectComponent>() && !event.victim.GetComponent<StatusEffectComponent>().effects[projectileComponent.statsusEffect]){
                event.eventBus->EmitEvent<StatusEffectEvent>(event.victim, projectileComponent.statsusEffect, event.eventBus, event.registry, projectileComponent.SEdurationMS);    
            }
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

void DamageSystem::Update(float deltaTime, Entity player){
    // wis regen mp for player
    auto& hpmp = player.GetComponent<HPMPComponent>();
    const auto& quiet = player.GetComponent<StatusEffectComponent>().effects[QUIET];
    if(!quiet && hpmp.activemp < hpmp.maxmp){
        hpmp.activemp += .12 * (hpmp.activewisdom + 8.3) * deltaTime/1000;
        if(hpmp.activemp > hpmp.maxmp){
            hpmp.activemp = hpmp.maxmp;
        }
    } 

    // vit regen hp for everyone
    for(auto entity: GetSystemEntities()){
        auto& stats = entity.GetComponent<HPMPComponent>();
        auto& activehp = stats.activehp;
        const auto& maxhp = stats.maxhp;
        const auto& activevitality = stats.activevitality;

        if((activehp < maxhp) && (activehp > 0)){
            activehp += ((1 + .24 * activevitality)/1000) * deltaTime;
            if(activehp > maxhp){
                activehp = maxhp;
            }
        }
        
    }
}