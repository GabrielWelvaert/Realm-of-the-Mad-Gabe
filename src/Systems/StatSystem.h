#ifndef STATSYSTEM_H
#define STATSYSTEM_H

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
#include "../Events/UpdateDisplayStatEvent.h"
#include "../Utils/tables.h"

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

    public:
        StatSystem(){
            RequireComponent<BaseStatComponent>();
        }

        // TODO: event stuff
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<LevelUpEvent>(this, &StatSystem::onLevelUp);
        }

        void onDrinkStatPot(){
            //todo
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


#endif