#pragma once 

#include "../ECS/ECS.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../EventBus/EventBus.h"
#include "../Utils/Xoshiro256.h"
#include "../Utils/enums.h"
#include "../Events/LevelUpEvent.h"
#include "../Events/EquipItemWithStatsEvent.h"
#include "../Events/UpdateDisplayStatEvent.h"
#include "../Events/DrinkConsumableEvent.h"
#include "../Utils/tables.h"
#include "../Utils/colors.h"

/*
this system is responsbile for managing the player stat increases at level up and potion drinking via events
*/

class StatSystem: public System{
    private:

        Xoshiro256 RNG;

        std::unordered_map<classes, std::array<std::array<char, 2>, 8>> levelUpRanges = {
            // HP, MP, ATTACK, def, SPEED, DEXTERITY, VITALITY, WISDOM 
            {ARCHER, {{{20,30},{2,8},{1,2},{0,0},{0,2},{0,2},{0,1},{0,2}}}},
            {PRIEST, {{{20,30},{5,15},{0,2},{0,0},{1,2},{0,2},{0,1},{1,2}}}},
            {WARRIOR, {{{20,30},{2,8},{1,2},{0,0},{0,2},{0,2},{1,2},{0,2}}}},
        };

        inline int getMaxStat(classes c, stats s){
            return maxStats[c][s];
        }

        // dont pass DEFENSE to this
        inline unsigned int getStatLevelUpAmount(classes c, stats s){
            int min = levelUpRanges[c][s][0];
            int max = levelUpRanges[c][s][1];
            return RNG.randomFromRange(min, max);
        }

        inline void displayHealText(std::unique_ptr<Registry>& registry, const glm::vec2& playerPosition, const int& healAmount){
            Entity dmgText = registry->CreateEntity();
            dmgText.AddComponent<TextLabelComponent>(
                "+" + std::to_string(healAmount),
                "damagefont",
                xpgreen,
                false,
                350,
                0,
                1
                );
            dmgText.AddComponent<TransformComponent>(playerPosition);
        }

        inline void displayMpHealText(std::unique_ptr<Registry>& registry, const glm::vec2& playerPosition, const int& healAmount){
            Entity dmgText = registry->CreateEntity();
            dmgText.AddComponent<TextLabelComponent>(
                "+" + std::to_string(healAmount),
                "damagefont",
                mpcolor,
                false,
                350,
                0,
                1
                );
            dmgText.AddComponent<TransformComponent>(playerPosition);
        }


    public:
        StatSystem(){
            RequireComponent<BaseStatComponent>();
        }

        // TODO: event stuff
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<LevelUpEvent>(this, &StatSystem::onLevelUp);
            eventBus->SubscribeToEvent<EquipItemWithStatsEvent>(this, &StatSystem::onEquipItemWithStats);
            eventBus->SubscribeToEvent<DrinkConsumableEvent>(this, &StatSystem::onDrinkConsumablePot);
        }

        void onDrinkConsumablePot(DrinkConsumableEvent& event){
            auto& player = event.player;
            auto& inventory = player.GetComponent<PlayerItemsComponent>().inventory;
            switch(event.itemEnum){
                case HPPOT:{
                    auto& hpmp = player.GetComponent<HPMPComponent>();
                    if(hpmp.activehp == hpmp.maxhp){
                        event.assetstore->PlaySound(ERROR);
                        return;
                    }
                    hpmp.activehp += 100;
                    if(hpmp.activehp > hpmp.maxhp){
                        displayHealText(event.registry, player.GetComponent<TransformComponent>().position, 100 - (static_cast<int>(hpmp.activehp) - static_cast<int>(hpmp.maxhp)));
                        hpmp.activehp = hpmp.maxhp;
                    } else {
                        displayHealText(event.registry, player.GetComponent<TransformComponent>().position, 100);
                    }
                }break;
                case MPPOT: {
                    auto& hpmp = player.GetComponent<HPMPComponent>();
                    if(hpmp.activemp == hpmp.maxmp || player.GetComponent<StatusEffectComponent>().effects[QUIET]){
                        event.assetstore->PlaySound(ERROR);
                        return;
                    }
                    hpmp.activemp += 100;
                    if(hpmp.activemp > hpmp.maxmp){
                        displayMpHealText(event.registry, player.GetComponent<TransformComponent>().position, 100 - (static_cast<int>(hpmp.activemp) - static_cast<int>(hpmp.maxmp)));
                        hpmp.activemp = hpmp.maxmp;
                    } else {
                        displayMpHealText(event.registry, player.GetComponent<TransformComponent>().position, 100);
                    }
                }break;
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
                case CABERNET:{
                    auto& hpmp = player.GetComponent<HPMPComponent>();
                    if(hpmp.activehp == hpmp.maxhp){
                        event.assetstore->PlaySound(ERROR);
                        return;
                    }
                    hpmp.activehp += 150;
                    if(hpmp.activehp > hpmp.maxhp){
                        displayHealText(event.registry, player.GetComponent<TransformComponent>().position, 150 - (static_cast<int>(hpmp.activehp) - static_cast<int>(hpmp.maxhp)));
                        hpmp.activehp = hpmp.maxhp;
                    } else {
                        displayHealText(event.registry, player.GetComponent<TransformComponent>().position, 150);
                    }
                }break;
                case FIREWATER:{
                    auto& hpmp = player.GetComponent<HPMPComponent>();
                    if(hpmp.activehp == hpmp.maxhp){
                        event.assetstore->PlaySound(ERROR);
                        return;
                    }
                    hpmp.activehp += 230;
                    if(hpmp.activehp > hpmp.maxhp){
                        displayHealText(event.registry, player.GetComponent<TransformComponent>().position, 230 - (static_cast<int>(hpmp.activehp) - static_cast<int>(hpmp.maxhp)));
                        hpmp.activehp = hpmp.maxhp;
                    } else {
                        displayHealText(event.registry, player.GetComponent<TransformComponent>().position, 230);
                    }
                }break;

            }
            event.assetstore->PlaySound(POTION);
            if(event.inventory){ // item from inventory
                inventory.at(event.invSlot).Kill();
                inventory.erase(event.invSlot);
            } else { // item from loot bag
                auto& lootbag = event.registry->GetComponent<LootBagComponent>(player.GetComponent<PlayerItemsComponent>().IdOfOpenBag).contents;
                lootbag.at(event.invSlot).Kill();
                lootbag.erase(event.invSlot);
            }
        }

        void onEquipItemWithStats(EquipItemWithStatsEvent& event){
            auto& hpmp = event.player.GetComponent<HPMPComponent>();
            auto& offensestats = event.player.GetComponent<OffenseStatComponent>();
            auto& speed = event.player.GetComponent<SpeedStatComponent>().activespeed;
            auto& newItemStats = itemEnumToStatData.at(event.newItem);
            if(event.unequip){
                auto& oldItemStats = itemEnumToStatData.at(event.previousItem);
                hpmp.maxhp -= oldItemStats.hp;
                hpmp.maxmp -= oldItemStats.mp;
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
        }

        void onLevelUp(LevelUpEvent& event){
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
            
            // defense is not increased at level-up

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

            // restore active stats if they're below base (above base like berserk will be managed by buffSystem, still...)
            if(HPMPstats.activedefense < playerBaseStats.defense) HPMPstats.activedefense = playerBaseStats.defense;
            if(HPMPstats.activehp < HPMPstats.maxhp) HPMPstats.activehp = HPMPstats.maxhp;
            if(HPMPstats.activemp < HPMPstats.maxmp) HPMPstats.activemp = HPMPstats.maxmp;
            if(HPMPstats.activewisdom < playerBaseStats.wisdom) HPMPstats.activewisdom = playerBaseStats.wisdom;
            if(HPMPstats.activevitality < playerBaseStats.vitality) HPMPstats.activevitality = playerBaseStats.vitality;
            if(offensestats.activeattack < playerBaseStats.attack) offensestats.activeattack = playerBaseStats.attack;
            if(offensestats.activedexterity < playerBaseStats.dexterity) offensestats.activedexterity = playerBaseStats.dexterity;
            if(speed.activespeed < playerBaseStats.speed) speed.activespeed = playerBaseStats.speed;

            // update PEC repeatfrequency
            projectileRepeatFrequency = 1000 / (.08666 * offensestats.activedexterity + 1.5); //dex to shots per second / 1000
            frameSpeedRate = (.08666 * offensestats.activedexterity + 1.5) * 2; //dex to shorts per second * 2

            event.eventBus->EmitEvent<UpdateDisplayStatEvent>(event.player);
            // todo reset debuffs because leveling-up resets debuffs... 

            // std::cout << "Congratulations! You've reached level " << static_cast<int>(playerBaseStats.level) << ", with a total of " << playerBaseStats.xp << " xp" << std::endl;
        }

        void Update(){

        }

};
