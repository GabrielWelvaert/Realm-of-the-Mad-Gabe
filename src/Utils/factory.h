#pragma once 

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
#include "../../libs/SDL2/SDL.h"

/*
The factory class contains methods for spawning entities that represent important things such as 
lootbags, monsters, items, portals
*/


class Factory{
    private:

        inline void createMonster(std::unique_ptr<Registry>& registry, glm::vec2 spawnpoint, sprites spriteEnum){
            Entity enemy = registry->CreateEntity();
            enemy.Group(MONSTER);
            enemy.AddComponent<HPMPComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy);
            enemy.AddComponent<SpriteComponent>(spriteEnum);
            enemy.AddComponent<BoxColliderComponent>(spriteEnum);
            enemy.AddComponent<CollisionFlagComponent>();
            enemy.AddComponent<TransformComponent>(spawnpoint);

            enemyCategory enemyCat = spriteToEnemyCategory.at(spriteEnum);
            switch(enemyCat){
                case ASC: // animated shooting chase category
                    enemy.AddComponent<AnimationComponent>(spriteEnum);
                    enemy.AddComponent<AnimatedChaseAIComponent>(spriteEnum);
                    enemy.AddComponent<RidigBodyComponent>();
                    enemy.AddComponent<SpeedStatComponent>(spriteEnum);
                    enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
                    break;
                case AS: // animated shooting category
                    enemy.AddComponent<AnimationComponent>(spriteEnum);
                    enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
                    enemy.AddComponent<AnimatedNeutralAIComponent>(spriteEnum);
                    break;
                case SC: // shooting chase category
                    enemy.AddComponent<RidigBodyComponent>();
                    enemy.AddComponent<SpeedStatComponent>(spriteEnum);
                    enemy.AddComponent<ChaseAIComponent>(spriteEnum);
                    break;
                case T: // trap category
                    enemy.AddComponent<AnimationComponent>(1,4,0); // hard-coded!
                    enemy.AddComponent<TrapAIComponent>(spriteEnum);
                    break;
            }
        }

        inline Entity generateLootBag(std::unique_ptr<Registry>& registry, glm::vec2 spawnpoint, sprites spriteEnum){
            Entity lootbag = registry->CreateEntity();
            lootbag.AddComponent<SpriteComponent>(spriteEnum);
            lootbag.AddComponent<TransformComponent>(spawnpoint, glm::vec2(5.0,5.0));
            lootbag.AddComponent<BoxColliderComponent>(LOOTBAG);
            lootbag.AddComponent<LootBagComponent>();
            lootbag.Group(LOOTBAGGROUP);
            return lootbag;
        }

        inline void generateItemInBag(std::unique_ptr<Registry>& registry, items itemEnum, LootBagComponent& lbc){
            Entity item = registry->CreateEntity();
            item.AddComponent<SpriteComponent>(itemEnum);
            item.AddComponent<ItemComponent>(itemEnum, lbc.addItem(item), lbc.contents);
            item.Group(itemToGroup.at(itemEnum));

        }

        // todo: spawn portal ?

        // todo: spawn player 

        // todo: force spawn item in player inventory or equipment


    public:

        void spawnMonster(std::unique_ptr<Registry>& registry, glm::vec2 spawnpoint, sprites spriteEnum){
            createMonster(registry, spawnpoint, spriteEnum);
        }

        void createItemInBag(std::unique_ptr<Registry>& registry, items itemEnum, LootBagComponent& lbc){
            return generateItemInBag(registry, itemEnum, lbc);
        }

        Entity creatLootBag(std::unique_ptr<Registry>& registry, glm::vec2 spawnpoint, sprites spriteEnum){
            return generateLootBag(registry, spawnpoint, spriteEnum);
        }
        
};