#ifndef FACTORY_H
#define FACTORY_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Components/DynamicUIEntityComponent.h"
#include "../Components/DisplayStatComponent.h"
#include "../Components/AnimatedShootingComponent.h"
#include "../Components/CollisionFlagComponent.h"
#include "../Components/LootBagComponent.h"
#include "../Components/InteractUIComponent.h"
#include "../Components/ItemComponent.h"
#include "../Components/PlayerItemsComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimatedChaseAIComponent.h"
#include "../Components/AnimatedNeutralAIComponent.h"
#include "../Components/ChaseAIComponent.h"
#include "../Components/TrapAIComponent.h"
#include "../Components/ItemTableComponent.h"
#include "../Components/StatusEffectComponent.h"
#include "../Components/PortalComponent.h"
#include "../../libs/SDL2/SDL.h"
#include "../Utils/Xoshiro256.h"
#include "../Utils/tables.h"
#include "../Utils/CharacterManager.h"
#include "../AssetStore/AssetStore.h"

/*
The factory class contains methods for spawning entities that represent important things such as 
lootbags, monsters, items, portals
*/

class Factory{
    private:
        Xoshiro256 RNG;

    public:
    
        inline Entity spawnMonster(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum){
            Entity enemy = registry->CreateEntity();
            enemy.Group(MONSTER);
            enemy.AddComponent<HPMPComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy);
            enemy.AddComponent<SpriteComponent>(spriteEnum);
            enemy.AddComponent<BoxColliderComponent>(spriteEnum);
            enemy.AddComponent<CollisionFlagComponent>();
            enemy.AddComponent<TransformComponent>(spawnpoint);
            enemy.AddComponent<StatusEffectComponent>();
            enemy.AddComponent<SpeedStatComponent>(spriteEnum); // stationary enemies still have their speedstatcomp taken when hit by quivers

            enemyCategory enemyCat = spriteToEnemyCategory.at(spriteEnum);
            switch(enemyCat){
                case ASC: // animated shooting chase category
                    enemy.AddComponent<AnimationComponent>(spriteEnum);
                    enemy.AddComponent<AnimatedChaseAIComponent>(spriteEnum);
                    enemy.AddComponent<RidigBodyComponent>();
                    enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
                    enemy.AddComponent<ItemTableComponent>(spriteEnum);
                    break;
                case AS: // animated shooting category
                    enemy.AddComponent<AnimationComponent>(spriteEnum);
                    enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
                    enemy.AddComponent<AnimatedNeutralAIComponent>(spriteEnum);
                    enemy.AddComponent<ItemTableComponent>(spriteEnum);
                    break;
                case SC: // shooting chase category
                    enemy.AddComponent<RidigBodyComponent>();
                    enemy.AddComponent<ChaseAIComponent>(spriteEnum);
                    enemy.AddComponent<ItemTableComponent>(spriteEnum);
                    break;
                case T: // trap category
                    enemy.AddComponent<AnimationComponent>(1,4,0); // hard-coded!
                    enemy.AddComponent<TrapAIComponent>(spriteEnum);
                    break;
            }

            return enemy;
        }

        inline Entity creatLootBag(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum){
            Entity lootbag = registry->CreateEntity();
            lootbag.AddComponent<SpriteComponent>(spriteEnum);
            lootbag.AddComponent<TransformComponent>(spawnpoint, glm::vec2(5.0,5.0));
            lootbag.AddComponent<BoxColliderComponent>(LOOTBAG);
            lootbag.AddComponent<LootBagComponent>();
            lootbag.Group(LOOTBAGGROUP);
            return lootbag;
        }

        inline void createItemInBag(std::unique_ptr<Registry>& registry, const items& itemEnum, Entity& lootbag){
            Entity item = registry->CreateEntity();
            auto& lbc = lootbag.GetComponent<LootBagComponent>();
            item.AddComponent<SpriteComponent>(itemEnum);
            item.AddComponent<ItemComponent>(itemEnum, lbc.addItem(item), lootbag.GetId());
            item.Group(itemToGroup.at(itemEnum));

        }

        inline void createLootAtDeath(Entity& monster, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore){
            //optimize: dont make a bag if its not going to have any loot. store roll values. then create bag if we got something. also pass monster by const ref/
            Entity lootbag = registry->CreateEntity();
            lootbag.Group(LOOTBAGGROUP);
            lootbag.AddComponent<LootBagComponent>();
            auto& lbc = lootbag.GetComponent<LootBagComponent>();
            const auto& tables = monster.GetComponent<ItemTableComponent>().dropTable;
            int roll;
            sprites bagsprite = BROWNLOOTBAG;
            for(const auto& table: tables){
                roll = RNG.randomFromRange(1,100);
                if(roll <= table.first){ // if value is lower than table rate, table is hit so random item from it
                    const auto& item = table.second[RNG.randomFromRange(0,table.second.size()-1)];
                    createItemInBag(registry, item, lootbag);
                    itemEnumToLootBagSpriteEnum.at(item) > bagsprite ? bagsprite = itemEnumToLootBagSpriteEnum.at(item) : bagsprite = bagsprite;
                }
            }
            if(lbc.contents.size() > 0){
                lootbag.AddComponent<TransformComponent>(monster.GetComponent<TransformComponent>().position, glm::vec2(5.0,5.0));
                lootbag.AddComponent<BoxColliderComponent>(LOOTBAG);
                lootbag.AddComponent<SpriteComponent>(bagsprite);
                assetstore->PlaySound(LOOT);
            } else { //empty bag; kill it
                lootbag.Kill();
            }
        }

        inline void spawnPortal(std::unique_ptr<Registry>& registry, glm::vec2 spawnpoint, wallTheme area){
            Entity portal = registry->CreateEntity();
            const auto& sprite = wallThemeToSpriteData.at(area);
            portal.Group(PORTAL);
            portal.AddComponent<SpriteComponent>(sprite.assetId, sprite.width, sprite.height, sprite.srcRect, sprite.zIndex, sprite.isFixed, sprite.diagonalSprite);
            portal.AddComponent<TransformComponent>(spawnpoint);
            portal.AddComponent<BoxColliderComponent>(LOOTBAG); // same size, its fine
            portal.AddComponent<PortalComponent>(area);
        }

        inline void spawnVaultChests(std::unique_ptr<Registry>& registry, std::unique_ptr<CharacterManager>& CharacterManager){
            CharacterManager->KillInvalidVaultFiles();
            for(int i = 0; i <= 2; i++){
                std::vector<int> itemInts = CharacterManager->GetItemsFromVault(i+1);
                glm::vec2 spawn = vaultSpawns[i];
                Entity vaultChest = creatLootBag(registry, spawn, VAULTCHEST);
                vaultChest.GetComponent<TransformComponent>().scale = glm::vec2(6.0,6.0);
                vaultChest.AddComponent<VaultChestComponent>(i+1);
                auto& contents = vaultChest.GetComponent<LootBagComponent>().contents;
                for(int j = 1; j <= 8; j++){
                    int currentInt = itemInts[j-1];
                    if(currentInt != -1){
                        items itemEnum = static_cast<items>(currentInt);    
                        Entity item = registry->CreateEntity();
                        // std::cout << "vault " << i << " item " << j << " has id " << item.GetId() << std::endl;
                        item.AddComponent<SpriteComponent>(itemEnum);
                        item.AddComponent<ItemComponent>(itemEnum, j, vaultChest.GetId());
                        item.Group(itemToGroup.at(itemEnum));
                        contents[j] = item;
                    }
                }
            }

        }
};

#endif