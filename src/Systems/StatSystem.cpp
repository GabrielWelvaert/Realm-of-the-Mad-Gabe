#include "StatSystem.h"

StatSystem::StatSystem(){
    RequireComponent<BaseStatComponent>();
}

void StatSystem::SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
    eventBus->SubscribeToEvent<LevelUpEvent>(this, &StatSystem::onLevelUp);
    eventBus->SubscribeToEvent<EquipItemWithStatsEvent>(this, &StatSystem::onEquipItemWithStats);
    eventBus->SubscribeToEvent<DrinkConsumableEvent>(this, &StatSystem::onDrinkConsumablePot);
}

bool StatSystem::increaseHP(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, int amount){
    auto& hpmp = player.GetComponent<HPMPComponent>();
    if(hpmp.activehp == hpmp.maxhp){
        assetstore->PlaySound(ERROR);
        return false; // false indicates invalid consumption attempt; exit onDrinkConsumablePot event
    }
    hpmp.activehp += amount;
    if(hpmp.activehp > hpmp.maxhp){
        displayHealText(registry, player.GetComponent<TransformComponent>().position, amount - (static_cast<int>(hpmp.activehp) - static_cast<int>(hpmp.maxhp)), player);
        hpmp.activehp = hpmp.maxhp;
    } else {
        displayHealText(registry, player.GetComponent<TransformComponent>().position, amount, player);
    }
    return true;
}

bool StatSystem::increaseMP(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, int amount){
    auto& hpmp = player.GetComponent<HPMPComponent>();
    if(hpmp.activemp == hpmp.maxmp || player.GetComponent<StatusEffectComponent>().effects[QUIET]){
        assetstore->PlaySound(ERROR);
        return false;
    }
    hpmp.activemp += amount;
    if(hpmp.activemp > hpmp.maxmp){
        displayMpHealText(registry, player.GetComponent<TransformComponent>().position, amount - (static_cast<int>(hpmp.activemp) - static_cast<int>(hpmp.maxmp)), player);
        hpmp.activemp = hpmp.maxmp;
    } else {
        displayMpHealText(registry, player.GetComponent<TransformComponent>().position, amount, player);
    }
    return true;
}

void StatSystem::onDrinkConsumablePot(DrinkConsumableEvent& event){
    auto& player = event.player;
    auto& playerIC = player.GetComponent<PlayerItemsComponent>();
    auto& inventory = playerIC.inventory;
    switch(event.itemEnum){
        case SPDTINCTURE:{
            event.eventbus->EmitEvent<StatusEffectEvent>(player, SPEEDY, event.eventbus, event.registry, 4000);
        } break;
        case DEFTINCTURE:{
            event.eventbus->EmitEvent<StatusEffectEvent>(player, ARMORED, event.eventbus, event.registry, 4000);
        } break;
        case GORDONINCANTATION:{
            event.factory->spawnPortal(event.registry, player.GetComponent<TransformComponent>().position, GORDONSLAIRWALLTHEME);
        } break;
        case HPPOT:{
            if(!increaseHP(player, event.registry, event.assetstore, 100)){
                return; // invalid consumption attempt; exit onDrinkConsumablePot event
            }
        }break;
        case MPPOT: {
            if(!increaseMP(player, event.registry, event.assetstore, 100)){
                return; // invalid consumption attempt; exit onDrinkConsumablePot event
            }
        }break;
        case CABERNET:{
            if(!increaseHP(player, event.registry, event.assetstore, 150)){
                return; // invalid consumption attempt; exit onDrinkConsumablePot event
            }
        }break;
        case FIREWATER:{
            if(!increaseHP(player, event.registry, event.assetstore, 230)){
                return; // invalid consumption attempt; exit onDrinkConsumablePot event
            }
        }break;
        case MINORHPPOT:{
            if(!increaseHP(player, event.registry, event.assetstore, 50)){
                return; // invalid consumption attempt; exit onDrinkConsumablePot event
            }
        } break;
        case MINORMPPOT:{
            if(!increaseMP(player, event.registry, event.assetstore, 50)){
                return; // invalid consumption attempt; exit onDrinkConsumablePot event
            }
        } break;
        case CUBEJUICE:{
            auto& hpmp = player.GetComponent<HPMPComponent>();
            if(hpmp.activehp == hpmp.maxhp && hpmp.activemp == hpmp.maxmp){
                event.assetstore->PlaySound(ERROR);
                return;
            }
            hpmp.activehp += 120;
            if(hpmp.activehp > hpmp.maxhp){
                displayHealText(event.registry, player.GetComponent<TransformComponent>().position, 120 - (static_cast<int>(hpmp.activehp) - static_cast<int>(hpmp.maxhp)), player);
                hpmp.activehp = hpmp.maxhp;
            } else {
                displayHealText(event.registry, player.GetComponent<TransformComponent>().position, 120, player);
            }
            hpmp.activemp += 120;
            if(hpmp.activemp > hpmp.maxmp){
                displayMpHealText(event.registry, player.GetComponent<TransformComponent>().position, 120 - (static_cast<int>(hpmp.activemp) - static_cast<int>(hpmp.maxmp)), player);
                hpmp.activemp = hpmp.maxmp;
            } else {
                displayMpHealText(event.registry, player.GetComponent<TransformComponent>().position, 120, player);
            }
        } break;
        case FLAMINGFLASK:{
            auto& hpmp = player.GetComponent<HPMPComponent>();
            if(hpmp.activehp <= 15){
                displayDamgeText(event.registry, event.player.GetComponent<TransformComponent>().position, hpmp.activehp, player);
                hpmp.activehp = 0.0;
            } else{
                displayDamgeText(event.registry, event.player.GetComponent<TransformComponent>().position, 15, player);
                hpmp.activehp -= 15;
            }
            event.eventbus->EmitEvent<StatusEffectEvent>(player, INVISIBLE, event.eventbus, event.registry, 4000);
            event.assetstore->PlaySound(RNG.randomFromVector(sounds));
        } break;
        case ATTPOT:{
            auto& basestats = player.GetComponent<BaseStatComponent>();
            auto& offensestats = player.GetComponent<OffenseStatComponent>();
            if(basestats.attack == getMaxStat(player.GetComponent<ClassNameComponent>().classname, ATTACK)){
                event.assetstore->PlaySound(ERROR);
                return;
            }
            basestats.attack ++;
            offensestats.activeattack ++;
            event.eventbus->EmitEvent<UpdateDisplayStatEvent>(player);

        }break;
        case DEFPOT:{
            auto& hpmp = player.GetComponent<HPMPComponent>();
            auto& basestats = player.GetComponent<BaseStatComponent>();
            if(basestats.defense == getMaxStat(player.GetComponent<ClassNameComponent>().classname, DEFENSE)){
                event.assetstore->PlaySound(ERROR);
                return;
            }
            basestats.defense ++;
            hpmp.activedefense ++;
            event.eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
        }break;
        case DEXPOT:{
            auto& offensestats = player.GetComponent<OffenseStatComponent>();
            auto& basestats = player.GetComponent<BaseStatComponent>();
            if(basestats.dexterity == getMaxStat(player.GetComponent<ClassNameComponent>().classname, DEXTERITY)){
                event.assetstore->PlaySound(ERROR);
                return;
            }
            basestats.dexterity ++;
            offensestats.activedexterity ++;
            auto& projectileRepeatFrequency = event.player.GetComponent<ProjectileEmitterComponent>().repeatFrequency;
            auto& frameSpeedRate = event.player.GetComponent<AnimationComponent>().frameSpeedRate;
            projectileRepeatFrequency = 1000 / (.08666 * offensestats.activedexterity + 1.5); //dex to shots per second / 1000
            frameSpeedRate = (.08666 * offensestats.activedexterity + 1.5) * 2; //dex to shorts per second * 2
            event.eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
        }break;
        case SPDPOT:{
            auto& activespeed = player.GetComponent<SpeedStatComponent>().activespeed;
            auto& basestats = player.GetComponent<BaseStatComponent>();
            if(basestats.speed == getMaxStat(player.GetComponent<ClassNameComponent>().classname, SPEED)){
                event.assetstore->PlaySound(ERROR);
                return;
            }
            basestats.speed ++;
            activespeed ++;
            event.eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
        }break;
        case WISPOT:{
            auto& hpmp = player.GetComponent<HPMPComponent>();
            auto& basestats = player.GetComponent<BaseStatComponent>();
            if(basestats.wisdom == getMaxStat(player.GetComponent<ClassNameComponent>().classname, WISDOM)){
                event.assetstore->PlaySound(ERROR);
                return;
            }
            basestats.wisdom ++;
            hpmp.activewisdom ++;
            event.eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
        }break;
        case VITPOT:{
            auto& hpmp = player.GetComponent<HPMPComponent>();
            auto& basestats = player.GetComponent<BaseStatComponent>();
            if(basestats.vitality == getMaxStat(player.GetComponent<ClassNameComponent>().classname, VITALITY)){
                event.assetstore->PlaySound(ERROR);
                return;
            }
            basestats.vitality ++;
            hpmp.activevitality ++;
            event.eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
        }break;
        case LIFEPOT:{
            auto& hpmp = player.GetComponent<HPMPComponent>();
            auto& basestats = player.GetComponent<BaseStatComponent>();
            if(basestats.hp == getMaxStat(player.GetComponent<ClassNameComponent>().classname, HP)){
                event.assetstore->PlaySound(ERROR);
                return;
            }
            basestats.hp += 5;
            hpmp.maxhp += 5;
            if(basestats.hp > getMaxStat(player.GetComponent<ClassNameComponent>().classname, HP)){
                basestats.hp = getMaxStat(player.GetComponent<ClassNameComponent>().classname, HP);
            }
            event.eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
        }break;
        case MANAPOT:{
            auto& hpmp = player.GetComponent<HPMPComponent>();
            auto& basestats = player.GetComponent<BaseStatComponent>();
            if(basestats.mp == getMaxStat(player.GetComponent<ClassNameComponent>().classname, MP)){
                event.assetstore->PlaySound(ERROR);
                return;
            }
            basestats.mp += 5;
            hpmp.maxmp += 5;
            if(basestats.mp > getMaxStat(player.GetComponent<ClassNameComponent>().classname, MP)){
                basestats.mp = getMaxStat(player.GetComponent<ClassNameComponent>().classname, MP);
            }
            event.eventbus->EmitEvent<UpdateDisplayStatEvent>(player);
        }break;
    }
    event.assetstore->PlaySound(POTION);
    playerIC.shiftblock = false;
    if(event.inventory){ // item from inventory
        inventory.at(event.invSlot).Kill();
        inventory.erase(event.invSlot);
    } else { // item from loot bag
        auto& lootbag = event.registry->GetComponent<LootBagComponent>(player.GetComponent<PlayerItemsComponent>().IdOfOpenBag).contents;
        lootbag.at(event.invSlot).Kill();
        lootbag.erase(event.invSlot);
    }
}


void StatSystem::onLevelUp(LevelUpEvent& event){
    auto& playerBaseStats = event.player.GetComponent<BaseStatComponent>();
    const auto& classname = event.player.GetComponent<ClassNameComponent>().classname;
    auto& HPMPstats = event.player.GetComponent<HPMPComponent>();
    auto& offensestats = event.player.GetComponent<OffenseStatComponent>();
    auto& speed = event.player.GetComponent<SpeedStatComponent>();
    auto& projectileRepeatFrequency = event.player.GetComponent<ProjectileEmitterComponent>().repeatFrequency;
    auto& frameSpeedRate = event.player.GetComponent<AnimationComponent>().frameSpeedRate;

    unsigned char hpincrease = getStatLevelUpAmount(classname, HP); 
    playerBaseStats.hp += hpincrease;
    if(playerBaseStats.hp > getMaxStat(classname, HP)) playerBaseStats.hp = getMaxStat(classname, HP); 
    
    unsigned char mpincrease = getStatLevelUpAmount(classname, MP); 
    playerBaseStats.mp += mpincrease;
    if(playerBaseStats.mp > getMaxStat(classname, MP)) playerBaseStats.mp = getMaxStat(classname, MP);
    
    unsigned char attackincrease = getStatLevelUpAmount(classname, ATTACK);
    playerBaseStats.attack += attackincrease;
    if(playerBaseStats.attack > getMaxStat(classname, ATTACK)) playerBaseStats.attack = getMaxStat(classname, ATTACK);
    
    unsigned char defincrease = getStatLevelUpAmount(classname, DEFENSE);
    if(classname == KNIGHT){
        playerBaseStats.defense += getStatLevelUpAmount(classname, DEFENSE);
        if(playerBaseStats.defense > getMaxStat(classname, DEFENSE)){
            playerBaseStats.defense = getMaxStat(classname, DEFENSE);
        }
    }

    unsigned char speedincrease = getStatLevelUpAmount(classname, SPEED); 
    playerBaseStats.speed += speedincrease;
    if(playerBaseStats.speed > getMaxStat(classname, SPEED)) playerBaseStats.speed = getMaxStat(classname, SPEED);
    
    unsigned char dexterityincrease = getStatLevelUpAmount(classname, DEXTERITY); 
    playerBaseStats.dexterity += dexterityincrease;
    if(playerBaseStats.dexterity > getMaxStat(classname, DEXTERITY)) playerBaseStats.dexterity = getMaxStat(classname, DEXTERITY);
    
    unsigned char vitalityincrease = getStatLevelUpAmount(classname, VITALITY); 
    playerBaseStats.vitality += vitalityincrease;
    if(playerBaseStats.vitality > getMaxStat(classname, VITALITY)) playerBaseStats.vitality = getMaxStat(classname, VITALITY);
    
    unsigned char wisdomincrease = getStatLevelUpAmount(classname, WISDOM); 
    playerBaseStats.wisdom += wisdomincrease;
    if(playerBaseStats.wisdom > getMaxStat(classname, WISDOM)) playerBaseStats.wisdom = getMaxStat(classname, WISDOM);

    playerBaseStats.level += 1;
    
    HPMPstats.maxhp += hpincrease;
    HPMPstats.maxmp += mpincrease;

    auto& sec = event.player.GetComponent<StatusEffectComponent>();
    if(!sec.effects.none()){
        if(sec.effects[SLOWED]){ // only debuff that modifies activestats; other's behavior is system-based
            speed.activespeed += sec.modifications[SLOWED];
            sec.effects[SLOWED] = false;
        }
        sec.effects[PARALYZE] = false;
        sec.effects[QUIET] = false;
        sec.effects[BLEEDING] = false;
        sec.effects[STUNNED] = false;
        sec.effects[BLIND] = false;
    }

    if(HPMPstats.activehp < HPMPstats.maxhp){
        HPMPstats.activehp = HPMPstats.maxhp;
    }
    if(HPMPstats.activemp < HPMPstats.maxmp){
        HPMPstats.activemp = HPMPstats.maxmp;
    } 
    HPMPstats.activewisdom += wisdomincrease;
    HPMPstats.activevitality += vitalityincrease;
    offensestats.activeattack += attackincrease;
    offensestats.activedexterity += dexterityincrease;
    speed.activespeed += speedincrease;
    HPMPstats.activedefense += defincrease;
    
    event.eventBus->EmitEvent<UpdateDisplayStatEvent>(event.player);

    // update PEC repeatfrequency
    projectileRepeatFrequency = 1000 / (.08666 * offensestats.activedexterity + 1.5); //dex to shots per second / 1000
    frameSpeedRate = (.08666 * offensestats.activedexterity + 1.5) * 2; //dex to shorts per second * 2
}

void StatSystem::onEquipItemWithStats(EquipItemWithStatsEvent& event){
    auto& hpmp = event.player.GetComponent<HPMPComponent>();
    auto& offensestats = event.player.GetComponent<OffenseStatComponent>();
    auto& speed = event.player.GetComponent<SpeedStatComponent>().activespeed;
    auto& newItemStats = itemEnumToStatData.at(event.newItem);
    if(event.unequip){
        auto& oldItemStats = itemEnumToStatData.at(event.previousItem);
        hpmp.maxhp -= oldItemStats.hp;
        hpmp.maxmp -= oldItemStats.mp;
        if(hpmp.activehp > hpmp.maxhp){
            hpmp.activehp = hpmp.maxhp;
        }
        if(hpmp.activemp > hpmp.maxmp){
            hpmp.activemp = hpmp.maxmp;
        }
        hpmp.activedefense -= oldItemStats.defense;
        hpmp.activevitality -= oldItemStats.vitality;
        hpmp.activewisdom -= oldItemStats.wisdom;
        speed -= oldItemStats.speed;
        offensestats.activeattack -= oldItemStats.attack;
        offensestats.activedexterity -= oldItemStats.dexterity;
    } 
    if(event.equip){
        hpmp.maxhp += newItemStats.hp;
        hpmp.maxmp += newItemStats.mp;
        hpmp.activedefense += newItemStats.defense;
        hpmp.activevitality += newItemStats.vitality;
        hpmp.activewisdom += newItemStats.wisdom;
        speed += newItemStats.speed;
        offensestats.activeattack += newItemStats.attack;
        offensestats.activedexterity += newItemStats.dexterity;
    }
    auto& projectileRepeatFrequency = event.player.GetComponent<ProjectileEmitterComponent>().repeatFrequency;
    auto& frameSpeedRate = event.player.GetComponent<AnimationComponent>().frameSpeedRate;
    projectileRepeatFrequency = 1000 / (.08666 * offensestats.activedexterity + 1.5); //dex to shots per second / 1000
    frameSpeedRate = (.08666 * offensestats.activedexterity + 1.5) * 2; //dex to shorts per second * 2
}