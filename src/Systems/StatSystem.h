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
#include "../Events/EquipItemWithStatsEvent.h"
#include "../Events/UpdateDisplayStatEvent.h"
#include "../Events/DrinkConsumableEvent.h"
#include "../Utils/tables.h"
#include "../Utils/colors.h"
#include "../Components/TransformComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/PlayerItemsComponent.h"
#include "../Components/StatusEffectComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/LootBagComponent.h"
#include "../Events/KillItemIconEvent.h"
#include "../Events/StatusEffectEvent.h"

/*
this system is responsbile for managing the player stat increases at level up and potion drinking via events
*/

class StatSystem: public System{
    private:

        Xoshiro256 RNG;
        
        inline bool increaseHP(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, int amount);

        inline bool increaseMP(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, int amount);

        std::vector<soundEnums> sounds = {ARCHERHIT,KNIGHTHIT,PALADINHIT,PRIESTHIT,ROGUEHIT,WARRIORHIT};

        std::unordered_map<classes, std::array<std::array<char, 2>, 8>> levelUpRanges = {
            // HP, MP, ATTACK, def, SPEED, DEXTERITY, VITALITY, WISDOM 
            {ARCHER, {{{22,30},{3,8},{1,2},{0,0},{1,2},{0,2},{1,3},{1,2}}}}, // for all, hp min used to be 20 (now 22) and mp min used to be 2 (now 3) or 5 (now 7)
            {PRIEST, {{{22,30},{7,15},{1,2},{0,0},{1,2},{0,2},{1,3},{1,3}}}},
            {WARRIOR, {{{22,30},{3,8},{1,2},{0,0},{1,2},{0,2},{1,3},{1,2}}}},
            {KNIGHT, {{{22,30},{3,8},{1,2},{0,1},{1,2},{0,2},{1,3},{1,2}}}},
            {WIZARD, {{{22,30},{7,15},{1,2},{0,0},{1,2},{1,2},{1,3},{1,2}}}},
            {ROGUE, {{{22,30},{3,8},{1,2},{0,0},{1,2},{1,2},{1,3},{1,2}}}},
            {NECROMANCER, {{{22,30},{7,15},{1,2},{0,0},{1,2},{1,2},{1,3},{1,2}}}},
            {PALADIN, {{{22,30},{3,8},{1,2},{0,0},{1,2},{0,2},{1,3},{1,3}}}},
            {SORCERER, {{{22,30},{7,15},{1,2},{0,0},{1,2},{1,2},{1,3},{1,2}}}},
        };

        inline int getMaxStat(classes c, stats s){
            return maxStats[c][s];
        }

        // dont pass DEFENSE to this unless its the knight
        inline unsigned int getStatLevelUpAmount(classes c, stats s){
            int min = levelUpRanges[c][s][0];
            int max = levelUpRanges[c][s][1];
            return RNG.randomFromRange(min, max);
        }

        inline void displayHealText(std::unique_ptr<Registry>& registry, const glm::vec2& playerPosition, const int& healAmount, const Entity& player){
            if(healAmount == 0){return;}
            Entity dmgText = registry->CreateEntity();
            dmgText.AddComponent<TextLabelComponent>(
                "+" + std::to_string(healAmount),
                "damagefont",
                xpgreen,
                false,
                350,
                player.GetId(),
                player.GetCreationId()
                );
            dmgText.AddComponent<TransformComponent>(playerPosition);
        }

        inline void displayMpHealText(std::unique_ptr<Registry>& registry, const glm::vec2& playerPosition, const int& healAmount, const Entity& player){
            if(healAmount == 0){return;}
            Entity dmgText = registry->CreateEntity();
            dmgText.AddComponent<TextLabelComponent>(
                "+" + std::to_string(healAmount),
                "damagefont",
                mpcolor,
                false,
                350,
                player.GetId(),
                player.GetCreationId()
                );
            dmgText.AddComponent<TransformComponent>(playerPosition);
        }

        inline void displayDamgeText(std::unique_ptr<Registry>& registry, const glm::vec2& victimPosition, const int& dmg, const Entity& player){
            Entity dmgText = registry->CreateEntity();
            dmgText.AddComponent<TextLabelComponent>(
                "-" + std::to_string(dmg),
                "damagefont",
                damagered,
                false,
                350,
                player.GetId(),
                player.GetCreationId()
                );
            dmgText.AddComponent<TransformComponent>(victimPosition);
        }


    public:
        StatSystem();

        int getNumberOfMaxStats(Entity player);

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus);
        
        void onDrinkConsumablePot(DrinkConsumableEvent& event);

        void onEquipItemWithStats(EquipItemWithStatsEvent& event);

        void onLevelUp(LevelUpEvent& event);

        void maxStat(Entity player, stats stat);
};

#endif