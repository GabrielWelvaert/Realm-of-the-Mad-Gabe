#include "StatusEffectSystem.h"

StatusEffectSystem::StatusEffectSystem(){
    RequireComponent<StatusEffectComponent>();
};

void StatusEffectSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<StatusEffectEvent>(this, &StatusEffectSystem::onStatusEnable);
}

void StatusEffectSystem::onStatusEnable(StatusEffectEvent& event){ // modify stats if necessary, save modified value for reversal later
    auto& entity = event.recipient;
    auto& sec = entity.GetComponent<StatusEffectComponent>();            
    const auto& statusEnum = event.statusEffectEnum;
    if(sec.effects[statusEnum]){ // if already enabled, just update duration
        sec.endTimes[statusEnum] = SDL_GetTicks() + event.duration;
        return;
    } 
    const auto& duration = event.duration;
    sec.set(statusEnum, duration);
    switch(statusEnum){
        case QUIET:{ 
            if(entity.BelongsToGroup(PLAYER)){
                // const auto& basestats = entity.GetComponent<BaseStatComponent>();
                auto& hpmp = entity.GetComponent<HPMPComponent>();
                hpmp.activemp = 0;
            }
            // monsters dont use mana
            displayStatusEffectText(event.registry, statusEnum, entity);
        }break;
        case SLOWED:{
            auto& activespeed = entity.GetComponent<SpeedStatComponent>().activespeed;
            if(entity.BelongsToGroup(PLAYER)){
                const auto& basestats = entity.GetComponent<BaseStatComponent>();
                activespeed -= basestats.speed / 2;
                sec.modifications[SLOWED] = basestats.speed / 2;
            } else {
                // for monsters (no base stat) you MUST save modification amount before modifying!
                sec.modifications[SLOWED] = activespeed / 2;
                activespeed -= activespeed / 2;
            }
            displayStatusEffectText(event.registry, statusEnum, entity);
        }break;
        case PARALYZE:{
            displayStatusEffectText(event.registry, statusEnum, entity);                 
        }break;
        case SPEEDY:{
            // if(!entity.HasComponent<SpeedStatComponent>()){
            //     return;
            // }
            auto& activespeed = entity.GetComponent<SpeedStatComponent>().activespeed;
            if(entity.BelongsToGroup(PLAYER)){
                const auto& basestats = entity.GetComponent<BaseStatComponent>();
                activespeed += basestats.speed / 2;
                sec.modifications[SPEEDY] = basestats.speed / 2;
            } else {
                // for monsters (no base stat) you MUST save modification amount before modifying!
                sec.modifications[SPEEDY] = activespeed / 2;
                activespeed += activespeed / 2;
            }
        }break;
        case BERSERK:{ // monsters dont have offensive stats
            if(entity.BelongsToGroup(PLAYER)){
                const auto& basestats = entity.GetComponent<BaseStatComponent>();
                auto& offensestats = entity.GetComponent<OffenseStatComponent>();
                offensestats.activedexterity += basestats.dexterity / 2;
                sec.modifications[BERSERK] = basestats.dexterity / 2;
                auto& projectileRepeatFrequency = entity.GetComponent<ProjectileEmitterComponent>().repeatFrequency;
                auto& frameSpeedRate = entity.GetComponent<AnimationComponent>().frameSpeedRate;
                projectileRepeatFrequency = 1000 / (.08666 * offensestats.activedexterity + 1.5); 
                frameSpeedRate = (.08666 * offensestats.activedexterity + 1.5) * 2; 
            }
        }break;
        case CONFUSED:{
            displayStatusEffectText(event.registry, statusEnum, entity); 
        }break;
        case BLEEDING:{
            displayStatusEffectText(event.registry, statusEnum, entity); 
        }break;
    }
    if(entity.BelongsToGroup(PLAYER)){
        event.eventbus->EmitEvent<UpdateDisplayStatEvent>(entity);    
    }
}

void StatusEffectSystem::onStatusDisable(Entity& recipient, const int& statusEnum, std::unique_ptr<EventBus>& eventbus){ // revert changes if necessary, called from this system's Update()
    auto& sec = recipient.GetComponent<StatusEffectComponent>();
    switch(statusEnum){
        case 0: // QUIET
        case 2: // PARALYZE
        case 5: // CONFUSED
        case 6: // BLEEDING
        case 7:{// INVULNERABLE
            // these dont modify stats; nothing to reset
            return; 
        }break;
        case 1:{// SLOWED 
            auto& activespeed = recipient.GetComponent<SpeedStatComponent>().activespeed;
            activespeed += sec.modifications[SLOWED];
        }break;
        case 3:{ //SPEEDY
            auto& activespeed = recipient.GetComponent<SpeedStatComponent>().activespeed;
            activespeed -= sec.modifications[SPEEDY];
        }break;
        case 4:{ //BERSERK
            if(recipient.BelongsToGroup(PLAYER)){
                const auto& basestats = recipient.GetComponent<BaseStatComponent>();
                auto& offensestats = recipient.GetComponent<OffenseStatComponent>();
                offensestats.activedexterity -= sec.modifications[BERSERK];
                auto& projectileRepeatFrequency = recipient.GetComponent<ProjectileEmitterComponent>().repeatFrequency;
                auto& frameSpeedRate = recipient.GetComponent<AnimationComponent>().frameSpeedRate;
                projectileRepeatFrequency = 1000 / (.08666 * offensestats.activedexterity + 1.5); 
                frameSpeedRate = (.08666 * offensestats.activedexterity + 1.5) * 2; 
            }
        } 
        
    }
    if(recipient.BelongsToGroup(PLAYER)){
        eventbus->EmitEvent<UpdateDisplayStatEvent>(recipient);    
    }
}

void StatusEffectSystem::Update(std::unique_ptr<EventBus>& eventbus){
    Uint32 currentTime = SDL_GetTicks();
    for(auto& entity: GetSystemEntities()){
        auto& sec = entity.GetComponent<StatusEffectComponent>();
        if(!sec.effects.none()){ // if bitset is not off 
            for(int i = 0; i <= 7; i++){
                if(sec.effects[i]){
                    if(i == BLEEDING && currentTime >= sec.lastBleedTime + 250){ // bleeding logic done here for cache-friendliness
                        auto& activehp = entity.GetComponent<HPMPComponent>().activehp;
                        if(activehp >= 5.0){ // player cannot die from bleeding
                            activehp -= 5.0;    
                        }
                        sec.lastBleedTime = currentTime;
                    }
                    if(currentTime >= sec.endTimes[i]){ // status effect expired; revert changes if necessary
                        sec.effects[i] = false;
                        onStatusDisable(entity, i, eventbus);
                    }
                }

            }    
        }
    }
}