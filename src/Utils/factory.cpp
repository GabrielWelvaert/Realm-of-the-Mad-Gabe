#include "factory.h"

Entity Factory::spawnMonster(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum){
        Entity enemy = registry->CreateEntity();
        enemy.Group(MONSTER);
        enemy.AddComponent<HPMPComponent>(spriteEnum);
        enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy);
        enemy.AddComponent<SpriteComponent>(spriteEnum);
        enemy.AddComponent<BoxColliderComponent>(spriteEnum);
        enemy.AddComponent<CollisionFlagComponent>();
        enemy.AddComponent<TransformComponent>(spawnpoint);
        enemy.AddComponent<StatusEffectComponent>();
        enemy.AddComponent<SpeedStatComponent>(spriteEnum, RNG); // stationary enemies still have their speedstatcomp taken when hit by quivers

        enemyCategory enemyCat = spriteToEnemyCategory.at(spriteEnum);
        switch(enemyCat){
            case ASC: // animated shooting chase category
                enemy.AddComponent<AnimationComponent>(spriteEnum);
                enemy.AddComponent<AnimatedChaseAIComponent>(spriteEnum, RNG);
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
                enemy.AddComponent<ChaseAIComponent>(spriteEnum, RNG);
                enemy.AddComponent<ItemTableComponent>(spriteEnum);
                break;
            case T: // trap category
                enemy.AddComponent<AnimationComponent>(1,4,0); // hard-coded; shatters bomb is only trap
                enemy.AddComponent<TrapAIComponent>(spriteEnum);
                break;
            case CHICKENBOSSAI:
                enemy.AddComponent<AnimationComponent>(spriteEnum);
                enemy.AddComponent<RidigBodyComponent>();
                enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
                enemy.AddComponent<ItemTableComponent>(spriteEnum);
                enemy.AddComponent<BossAIComponent>(BOSSCHICKEN, spawnpoint, 2000, 300, 700);
                enemy.GetComponent<TransformComponent>().position = enemy.GetComponent<BossAIComponent>().phaseOnePositions[0]; // for chicken circle
                break;
            case POUNCE:
                enemy.AddComponent<AnimationComponent>(spriteEnum);
                enemy.AddComponent<AnimatedPounceAIComponent>(spriteEnum, RNG);
                enemy.AddComponent<RidigBodyComponent>();
                enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
                enemy.AddComponent<ItemTableComponent>(spriteEnum);
                break;
            case ARCMAGEBOSSAI:
                enemy.AddComponent<AnimationComponent>(spriteEnum);
                enemy.AddComponent<RidigBodyComponent>();
                enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
                enemy.AddComponent<ItemTableComponent>(spriteEnum);
                enemy.AddComponent<BossAIComponent>(ARCMAGE, spawnpoint, 10000, 2500, 750);
                auto& sec = enemy.GetComponent<StatusEffectComponent>();
                sec.effects[INVULNERABLE] = true;
                sec.endTimes[INVULNERABLE] = 0-1;
                auto& ac = enemy.GetComponent<AnimationComponent>();
                ac.xmin = 0;
                ac.numFrames = 1;
                break;
        }

        return enemy;
}

void Factory::populateDungeonWithMonsters(std::unique_ptr<Registry>& registry, std::vector<room>& dungeonRooms, wallTheme dungeonType, int bossRoomId){
    for(const auto& room: dungeonRooms){
        if(room.id == 0){
            continue;
        } else if(room.id == bossRoomId){ // spawn boss in boss room
            glm::vec2 spawnPos = glm::vec2( ((room.x + (room.w / 2)) * 64)-48, ((room.y + (room.h / 2)) * 64)-48);
            switch(dungeonType){
                case CHICKENLAIR:{
                    spawnMonster(registry, spawnPos, BOSSCHICKEN);
                } break;
                case UDL:{
                    spawnMonster(registry, spawnPos, ARCMAGE);          
                } 
            }
            continue;
        }
        int minX = room.x + 2; // these values are in tiles
        int minY = room.y + 2;
        int maxX = room.x + room.w - 2;
        int maxY = room.y + room.h - 2;
        const auto& possibleRoomSpawns = wallThemeToMonsterSpawns.at(dungeonType); // the table that had possible room spawns; ex tiny red and white chickens together
        const auto& selectedRoomSpawns = possibleRoomSpawns[RNG.randomFromRange(0, possibleRoomSpawns.size()-1)];
        int roomQuantifier = (room.w + room.h) / 4; // ex 10x10 room will spawn 10 monsters per spawn type in selectedRoomSpawns
        for(const auto& enemySpawn: selectedRoomSpawns){
            int numToSpawn = enemySpawn.modifier * roomQuantifier;
            for(int i = 0; i <= numToSpawn; i++){
                // multiply spawnPos by 64 to convert from tile location to pixel location
                glm::vec2 spawnPos = {RNG.randomFromRange(minX, maxX) * 64, RNG.randomFromRange(minY, maxY) * 64};
                spawnMonster(registry, spawnPos, enemySpawn.monster);
            }
        }
    }
    registry->Update();

}

Entity Factory::creatLootBag(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum){
    Entity lootbag = registry->CreateEntity();
    lootbag.AddComponent<SpriteComponent>(spriteEnum);
    lootbag.AddComponent<TransformComponent>(spawnpoint, glm::vec2(5.0,5.0));
    lootbag.AddComponent<BoxColliderComponent>(LOOTBAG);
    lootbag.AddComponent<LootBagComponent>();
    lootbag.Group(LOOTBAGGROUP);
    return lootbag;
}

void Factory::createItemInBag(std::unique_ptr<Registry>& registry, const items& itemEnum, Entity& lootbag){
    Entity item = registry->CreateEntity();
    auto& lbc = lootbag.GetComponent<LootBagComponent>();
    item.AddComponent<SpriteComponent>(itemEnum);
    item.AddComponent<ItemComponent>(itemEnum, lbc.addItem(item), lootbag.GetId());
    item.Group(itemToGroup.at(itemEnum));

}

void Factory::createLootAtDeath(Entity& monster, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore){
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
        const auto& s = monster.GetComponent<SpriteComponent>();
        const auto& t = monster.GetComponent<TransformComponent>();
        glm::vec2 spawnpoint = {t.position.x + (s.width * t.scale.x / 2) - 20, t.position.y + (s.height * t.scale.y / 2) - 20};
        lootbag.AddComponent<TransformComponent>(spawnpoint, glm::vec2(5.0,5.0));
        lootbag.AddComponent<BoxColliderComponent>(LOOTBAG);
        lootbag.AddComponent<SpriteComponent>(bagsprite);
        assetstore->PlaySound(LOOT);
    } else { //empty bag; kill it
        lootbag.Kill();
    }
}

void Factory::spawnPortal(std::unique_ptr<Registry>& registry, glm::vec2 spawnpoint, wallTheme area){
    Entity portal = registry->CreateEntity();
    const auto& sprite = wallThemeToSpriteData.at(area);
    portal.Group(PORTAL);
    portal.AddComponent<SpriteComponent>(sprite.assetId, sprite.width, sprite.height, sprite.srcRect, sprite.zIndex, sprite.isFixed, sprite.diagonalSprite);
    portal.AddComponent<TransformComponent>(spawnpoint);
    portal.AddComponent<BoxColliderComponent>(LOOTBAG); // same size, its fine
    portal.AddComponent<PortalComponent>(area);
    if(area == CHANGECHAR){
        portal.GetComponent<SpriteComponent>().flip = SDL_FLIP_HORIZONTAL;
    }
}

void Factory::spawnVaultChests(std::unique_ptr<Registry>& registry, std::unique_ptr<CharacterManager>& CharacterManager){
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