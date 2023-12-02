#include "DamageSystem.h"

DamageSystem::DamageSystem(){
    RequireComponent<BoxColliderComponent>();
    RequireComponent<HPMPComponent>();
    RequireComponent<StatusEffectComponent>();
    RequireComponent<TransformComponent>();
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
    groups groupOfVictim = event.registry->IdToGroup(event.victim.GetId());
    unsigned short realdamage = projectileComponent.damage; // local copy of damage
    
    // first, check if this projectile collision needs to be processed and set flags if needed
    if(projectileComponent.damage == -1){return;} // flag
    if(projectileComponent.piercing){
        if(projectileVictimsAsCIDs[event.projectile.GetCreationId()].find(event.victim.GetCreationId()) != projectileVictimsAsCIDs[event.projectile.GetCreationId()].end()){
            return; // if this piercing projectile has already hit this target, return
        } else{
            projectileVictimsAsCIDs[event.projectile.GetCreationId()].emplace(event.victim.GetCreationId()); // record this target being victim of this piercing projectile for further reference
        }
    } else {
        projectileComponent.damage = -1; // this non-piercing projectile will still exist until end of frame (next registry update); mark it as -1 damage so it may not hit anyone else
        event.projectile.Kill();
    } 

    // defense & damage calculation
    if(invulnerable){
        realdamage = 0; // only case where 0 damage is permitted is when entity is invulnerable
    } else if(!projectileComponent.ignoresDefense){ // armor piercing shots skip this logic to do flat damage
        unsigned short projectileBaseDamage = realdamage;
        if(projectileBaseDamage >= victimHPMPComponent.activedefense){
            realdamage = projectileBaseDamage - victimHPMPComponent.activedefense;
        } else {
            realdamage = 0; // set to 0 instead of overflowing in cases where dmg < def 
        }
        // in rotmg, unless invulnerable, maximum damage reuction is 90%
        if(realdamage < projectileBaseDamage * .1){
            realdamage = projectileBaseDamage * .1;
            if(realdamage < 1){
                realdamage = 1;
            }
        }
    }

    // noise logic
    if(groupOfVictim == PLAYER){
        hitSoundId = playerHitSounds[RNG.randomFromRange(0,5)];
    } else { // monster is victim
        hitSoundId = victimHPMPComponent.hitsound;
        deathSoundId = victimHPMPComponent.deathsound;
    }

    // projectile damage, xp, death 
    victimHPMPComponent.activehp -= realdamage;
    if(projectileComponent.inflictsStatusEffect){ // invulnerablility does not grant immunity to status effects, that'd be intangibility
        event.eventBus->EmitEvent<StatusEffectEvent>(event.victim, projectileComponent.statsusEffect, event.eventBus, event.registry, projectileComponent.SEdurationMS);
    }
    if(victimHPMPComponent.activehp >= 0){ // victim hit but not dead (player or monster)
        event.assetStore->PlaySound(hitSoundId);
        if(realdamage > 0){
            displayDamgeText(event,victimPosition,realdamage);
        }
    } else { // hp has gone below 0
        switch(groupOfVictim){
            case MONSTER:{ // death of a monster 
                event.victim.Kill(); 
                event.assetStore->PlaySound(deathSoundId);  
                const auto& projectileParent = projectileComponent.parent;
                auto& playerBaseStats = projectileParent.GetComponent<BaseStatComponent>();
                int xp = victimHPMPComponent.maxhp / 10;
                if(xp > 0){ 
                    playerBaseStats.xp += xp;  
                    displayXPText(event, projectileParent.GetComponent<TransformComponent>().position , xp, projectileComponent.parent);
                    if(playerBaseStats.level < 20 && playerBaseStats.xp >= nextXPToLevelUp[playerBaseStats.level]){ // player level up
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
            } break;
            case PLAYER:{ // death of player 
                event.dp.className = event.victim.GetComponent<ClassNameComponent>().classname;
                event.dp.level = event.victim.GetComponent<BaseStatComponent>().level;
                event.dp.xp = event.victim.GetComponent<BaseStatComponent>().xp;
                event.dp.murderer = projectileComponent.spriteOfParent;
                event.characterManager->KillCharacter(event.activeCharacterID);
                event.registry->killAllEntities();
                event.assetStore->PlaySound(DEATH);
                event.Setup(false, true, NEXUS);
                return;
            } break;
        }
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
        const auto& activevitality = stats.activevitality;
        if(activevitality == 0){continue;}
        auto& activehp = stats.activehp;
        const auto& maxhp = stats.maxhp;

        if((activehp < maxhp) && (activehp > 0)){
            activehp += ((1 + .24 * activevitality)/250) * deltaTime;
            if(activehp > maxhp){
                activehp = maxhp;
            }
        }
    }
}