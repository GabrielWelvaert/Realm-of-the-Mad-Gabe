#include "StatusEffectSystem.h"

StatusEffectSystem::StatusEffectSystem(){
    RequireComponent<StatusEffectComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<SpriteComponent>();
};

void StatusEffectSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<StatusEffectEvent>(this, &StatusEffectSystem::onStatusEnable);
}

void StatusEffectSystem::GenerateStatusIcon(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, std::bitset<TOTAL_NUMBER_OF_STATUS_EFFECTS> bits){
    SDL_Rect dstRect;
    SDL_Texture * spriteAtlasTexture = assetStore->GetTexture(LOFIINTERFACE2); // returns a pointer to the .png's SDL_Texture *
    int numIconsToRender = bits.count();
    constexpr int totalNumberStatusEffects = static_cast<int>(TOTAL_NUMBER_OF_STATUS_EFFECTS);
    constexpr int iconDimension = 16;
    int width = (iconDimension*numIconsToRender) + numIconsToRender - 1;
    SDL_Texture* texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET, width, iconDimension);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    int iconsRendered = 0;
    for(int i = 0; i < totalNumberStatusEffects; i++){
        if(bits[i]){
            dstRect = {iconsRendered*iconDimension + 1 * (iconsRendered!=0),0,iconDimension,iconDimension};
            SDL_RenderCopy(renderer, spriteAtlasTexture, &icons[i], &dstRect); //src, dest
            iconsRendered++;
        }
    }
    effectTextures[bits.to_ulong()] = texture; // effectTextures is an unordered_map<unsigned long, SDL_Texture *>
    SDL_SetRenderTarget(renderer, nullptr);
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
                if(sec.effects[INVISIBLE]){ // getting hit with quiet disables invisibility
                    sec.effects[INVISIBLE] = false;
                    onStatusDisable(entity, INVISIBLE, event.eventbus);
                }
            }
            // monsters dont use mana
            displayStatusEffectText(event.registry, statusEnum, entity);
        }break;
        case SLOWED:{
            if(sec.effects[PARALYZE]){
                sec.effects[SLOWED] = false;
                return; // block slowed if paralyzed
            }
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
        case WEAKENED:{
            auto& activeattack = entity.GetComponent<OffenseStatComponent>().activeattack;
            if(entity.BelongsToGroup(PLAYER)){
                const auto& basestats = entity.GetComponent<BaseStatComponent>();
                activeattack -= basestats.attack / 2;
                sec.modifications[WEAKENED] = basestats.attack / 2;
            } else {
                // for monsters (no base stat) you MUST save modification amount before modifying!
                sec.modifications[WEAKENED] = activeattack / 2;
                activeattack -= activeattack / 2;
            }
            displayStatusEffectText(event.registry, statusEnum, entity); 
        } break;
        case ARMORBROKEN:{
            if(sec.effects[ARMORED]){
                sec.effects[ARMORED] = false;
                onStatusDisable(entity, ARMORED, event.eventbus);
            }

            auto& activedef = entity.GetComponent<HPMPComponent>().activedefense;
            sec.modifications[ARMORBROKEN] = activedef;
            activedef = 0;
            displayStatusEffectText(event.registry, statusEnum, entity);
        } break;
        case PARALYZE:{
            if(sec.effects[SLOWED]){
                sec.effects[SLOWED] = false;
                onStatusDisable(entity, SLOWED, event.eventbus);
            }
            if(sec.effects[SPEEDY]){
                sec.effects[SPEEDY] = false;
                onStatusDisable(entity, SPEEDY, event.eventbus);
            }
            auto& activespeed = entity.GetComponent<SpeedStatComponent>().activespeed;
            sec.modifications[PARALYZE] = activespeed;
            activespeed = 0;
            displayStatusEffectText(event.registry, statusEnum, entity);
        } break;
        case SPEEDY:{
            if(sec.effects[PARALYZE]){
                sec.effects[SPEEDY] = false;
                return; // block slowed if paralyzed
            }
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
        case ARMORED:{
            if(sec.effects[ARMORBROKEN]){
                sec.effects[ARMORED] = false;
                return; // block armored if entity is armor broken
            }
            auto& activedef = entity.GetComponent<HPMPComponent>().activedefense;
            if(entity.BelongsToGroup(PLAYER)){
                const auto& basestats = entity.GetComponent<BaseStatComponent>();
                if(basestats.defense < 2){
                    activedef += 2;
                    sec.modifications[ARMORED] = 2;
                } else {
                    activedef += basestats.defense / 2;
                    sec.modifications[ARMORED] = basestats.defense / 2;
                }
            } else {
                // for monsters (no base stat) you MUST save modification amount before modifying!
                sec.modifications[ARMORED] = activedef / 2;
                activedef += activedef / 2;
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
        case DAMAGING:{
            const auto& basestats = entity.GetComponent<BaseStatComponent>();
            auto& offensestats = entity.GetComponent<OffenseStatComponent>();
            offensestats.activeattack += basestats.attack / 2;
            sec.modifications[DAMAGING] = basestats.attack / 2;
        } break;
        case CONFUSED:
        case BLEEDING:
        case STUNNED:
        case BLIND: {
            displayStatusEffectText(event.registry, statusEnum, entity); 
        } break;
        case INVISIBLE:{
            if(entity.BelongsToGroup(PLAYER)){
                entity.GetComponent<SpriteComponent>().assetId = INVISIBLEPLAYERS;
            } else {
                // todo monster invisibility
            }
        }break;
    }
    if(entity.BelongsToGroup(PLAYER)){
        int mx,my;
        SDL_GetMouseState(&mx, &my);
        if(!(mx > 750 && mx < 1000 && my > 370 && my < 450)){ // player was not viewing stats to max...
            event.eventbus->EmitEvent<UpdateDisplayStatEvent>(entity);        
        }
    }
}

void StatusEffectSystem::onStatusDisable(Entity& recipient, statuses status, std::unique_ptr<EventBus>& eventbus){ // revert changes if necessary, called from this system's Update()
    auto& sec = recipient.GetComponent<StatusEffectComponent>();
    switch(status){
        case SLOWED:{// SLOWED 
            auto& activespeed = recipient.GetComponent<SpeedStatComponent>().activespeed;
            activespeed += sec.modifications[SLOWED];
        }break;
        case SPEEDY:{ //SPEEDY
            auto& activespeed = recipient.GetComponent<SpeedStatComponent>().activespeed;
            activespeed -= sec.modifications[SPEEDY];
        }break;
        case WEAKENED:{
            auto& activeattack = recipient.GetComponent<OffenseStatComponent>().activeattack;
            activeattack += sec.modifications[WEAKENED];
        } break;
        case ARMORED:{ //
            auto& activedef = recipient.GetComponent<HPMPComponent>().activedefense;
            activedef -= sec.modifications[ARMORED];
        }break;
        case DAMAGING:{
            auto& activeatt = recipient.GetComponent<OffenseStatComponent>().activeattack;
            activeatt -= sec.modifications[DAMAGING];
        } break;
        case BERSERK:{ //BERSERK
            if(recipient.BelongsToGroup(PLAYER)){ // ? 
                const auto& basestats = recipient.GetComponent<BaseStatComponent>();
                auto& offensestats = recipient.GetComponent<OffenseStatComponent>();
                offensestats.activedexterity -= sec.modifications[BERSERK];
                auto& projectileRepeatFrequency = recipient.GetComponent<ProjectileEmitterComponent>().repeatFrequency;
                auto& frameSpeedRate = recipient.GetComponent<AnimationComponent>().frameSpeedRate;
                projectileRepeatFrequency = 1000 / (.08666 * offensestats.activedexterity + 1.5); 
                frameSpeedRate = (.08666 * offensestats.activedexterity + 1.5) * 2; 
            }
        }break;
        case INVISIBLE:{
            if(recipient.BelongsToGroup(PLAYER)){
                recipient.GetComponent<SpriteComponent>().assetId = PLAYERS;
            } else {
                // todo monster invisibility if it exists
            }
        } break;
        case PARALYZE:{
            auto& activespeed = recipient.GetComponent<SpeedStatComponent>().activespeed;
            activespeed += sec.modifications[PARALYZE];
        } break;
        case ARMORBROKEN:{
            auto& activedef = recipient.GetComponent<HPMPComponent>().activedefense;
            activedef += sec.modifications[ARMORBROKEN];
        } break;
        default:{ // some status effects do not require anything to be reverted
        }break;
        
    }
    if(recipient.BelongsToGroup(PLAYER)){
        int mx,my;
        SDL_GetMouseState(&mx, &my);
        if(!(mx > 750 && mx < 1000 && my > 370 && my < 450)){ // player was not viewing stats to max...
            eventbus->EmitEvent<UpdateDisplayStatEvent>(recipient);        
        }  
    }
}

void StatusEffectSystem::killIconSetTextures(){ // not sure why valgrind reports invalid reads here!
    for(auto& pair: effectTextures){
        SDL_DestroyTexture(pair.second);
    }
}

void StatusEffectSystem::Update(SDL_Renderer* renderer, std::unique_ptr<EventBus>& eventbus, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera){
    Uint32 currentTime = SDL_GetTicks();
    for(auto& entity: GetSystemEntities()){
        auto& sec = entity.GetComponent<StatusEffectComponent>();
        if(!sec.effects.none()){ // if no statuses for this entity, continue
            for(int i = 0; i < TOTAL_NUMBER_OF_STATUS_EFFECTS; i++){ // 
                if(sec.effects[i]){
                    if(i == BLEEDING && currentTime >= sec.lastBleedTime + 250){ // bleeding logic done here for cache-friendliness
                        auto& activehp = entity.GetComponent<HPMPComponent>().activehp;
                        if(activehp >= 5.0){ // player cannot die from bleeding
                            activehp -= 5.0;    // 20 per second
                        }
                        sec.lastBleedTime = currentTime;
                    } else if(i == HEALING && currentTime >= sec.lastHealTime + 250){
                        auto& HPMP = entity.GetComponent<HPMPComponent>();
                        HPMP.activehp += 10.0; // 40 per second
                        if(HPMP.activehp > HPMP.maxhp){
                            HPMP.activehp = HPMP.maxhp;
                        }
                        sec.lastHealTime = currentTime;
                    }
                    if(currentTime >= sec.endTimes[i]){ // status effect expired; revert changes if necessary
                        sec.effects[i] = false;
                        onStatusDisable(entity, static_cast<statuses>(i), eventbus);
                        sec.modifications[i] = 0;
                    } 
                }
            }    

            if(!sec.effects.none()){ // if all statuses are now disabled, dont attempt to render icons
                SDL_Rect dstRect;
                int numIconsToRender = sec.effects.count();
                int iconDimension = 16;
                int width = (iconDimension*numIconsToRender) + numIconsToRender - 1;
                const auto& parentSprite = entity.GetComponent<SpriteComponent>();
                const auto& transform = entity.GetComponent<TransformComponent>();
                int xpos = static_cast<int>(transform.position.x + (static_cast<int>(parentSprite.width) * transform.scale.x / 2) - width / 2 - camera.x);
                if(xpos + width < 750){
                    dstRect = {xpos,static_cast<int>(transform.position.y - iconDimension - 2 - camera.y),width,iconDimension};
                    auto set = sec.effects.to_ulong();
                    if(effectTextures.find(set) == effectTextures.end()){
                        GenerateStatusIcon(renderer, assetStore, sec.effects);
                    }
                    SDL_RenderCopy(renderer, effectTextures[sec.effects.to_ulong()], NULL, &dstRect);
                }
            }
        }
    }
}