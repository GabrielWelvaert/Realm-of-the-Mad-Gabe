#include "factory.h"

Entity Factory::spawnMonster(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum, int parentId){
    Entity enemy = registry->CreateEntity();
    enemy.Group(MONSTER);
    enemy.AddComponent<HPMPComponent>(spriteEnum);
    enemy.AddComponent<SpriteComponent>(spriteEnum);
    enemy.GetComponent<SpriteComponent>().flip = flips[RNG.randomFromRange(0,1)]; // monsters spawns facing random direction
    enemy.AddComponent<BoxColliderComponent>(spriteEnum);
    enemy.AddComponent<CollisionFlagComponent>();
    enemy.AddComponent<StatusEffectComponent>();
    enemy.AddComponent<SpeedStatComponent>(spriteEnum, RNG); // stationary enemies still have their speedstatcomp taken when hit by quivers
    enemy.AddComponent<DistanceToPlayerComponent>();

    // very few monsters will requrie non-default (6.0) scaling
    switch(spriteEnum){
        case ORANGECUBE:{
            enemy.AddComponent<TransformComponent>(spawnpoint, glm::vec2(8.0));
        } break;
        case YELLOWCUBE:{
            enemy.AddComponent<TransformComponent>(spawnpoint, glm::vec2(7.0));
        } break;
        case CYANCUBE: {
            enemy.AddComponent<TransformComponent>(spawnpoint, glm::vec2(5.0));
        } break;
        default:{
            enemy.AddComponent<TransformComponent>(spawnpoint); // default = 6.0
        } break;
    }


    // if this monster has a secondary projectile
    if(hasSecondaryProjectile.find(spriteEnum) != hasSecondaryProjectile.end()){
        enemy.AddComponent<SecondaryProjectileComponent>(spriteEnum);
    }

    // if this monster must be grouped with a monster sub-group
    if(spriteToMonsterSubGroups.find(spriteEnum) != spriteToMonsterSubGroups.end()){
        monsterSubGroups msg = spriteToMonsterSubGroups.at(spriteEnum);
        enemy.monsterSubGroup(msg);
    }

    // if this monster should be able to spawn minions
    if(spriteToMinionSpawnerData.find(spriteEnum) != spriteToMinionSpawnerData.end()){
        const auto& msd = spriteToMinionSpawnerData.at(spriteEnum);
        enemy.AddComponent<MinionSpawnerComponent>(msd.monsterToSpawn, msd.maxMinions, msd.respawnInterval);
    }

    enemyCategory enemyCat = spriteToEnemyCategory.at(spriteEnum);
    switch(enemyCat){
        case MINION:{ // minions may do things like shoot but also want to stay near their parent
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<ItemTableComponent>(spriteEnum);
            const auto& minionType = spriteToMinionAIType.at(spriteEnum);
            enemy.AddComponent<MinionComponent>(minionType, parentId, registry->GetCreationIdFromEntityId(parentId));
            switch(minionType){
                case ORBIT:{ // no animation, no shooting
                    const auto& orbitData = spriteToOrbitalMovementData.at(spriteEnum);
                    auto distance = orbitData.distance * RNG.randomFromRange(1.0f, orbitData.distanceModMax);
                    enemy.AddComponent<OrbitalMovementComponent>(distance, RNG.randomBool());
                } break;
                case ORBIT_SHOOT:{ // no animation, does shoot
                    enemy.AddComponent<isShootingComponent>();
                    enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
                } break;
            }
            

        } break;
        case KEY:{ // keys are not minion-like because they dont shoot or anything
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<ItemTableComponent>(spriteEnum);
        } break;
        case ASC:{ // animated shooting chase category
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<AnimatedChaseAIComponent>(spriteEnum, RNG);
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<ItemTableComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<isShootingComponent>();
        } break;
        case AS:{ // animated shooting category
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<AnimatedNeutralAIComponent>(spriteEnum);
            enemy.AddComponent<ItemTableComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<isShootingComponent>();
        } break;
        case SC:{ // shooting chase category
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<ChaseAIComponent>(spriteEnum, RNG);
            enemy.AddComponent<ItemTableComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<isShootingComponent>();
        } break;
        case TRAP:{ // trap category
            enemy.AddComponent<AnimationComponent>(1,4,0); // hard-coded; shatters bomb is currently only trap
            enemy.AddComponent<TrapAIComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<isShootingComponent>();
        } break;
        case CHICKENBOSSAI:{
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<ItemTableComponent>(spriteEnum);
            enemy.AddComponent<BossAIComponent>(BOSSCHICKEN, spawnpoint, 2000, 300, 700);
            enemy.GetComponent<TransformComponent>().position = enemy.GetComponent<BossAIComponent>().phaseOnePositions[0]; // for chicken circle
            enemy.AddComponent<isShootingComponent>();
        } break;
        case ARCMAGEBOSSAI:{
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<ItemTableComponent>(spriteEnum);
            enemy.AddComponent<BossAIComponent>(ARCMAGE, spawnpoint, 20000, 3500, 750);
            auto& sec = enemy.GetComponent<StatusEffectComponent>();
            sec.effects[INVULNERABLE] = true;
            sec.endTimes[INVULNERABLE] = 0-1;
            auto& ac = enemy.GetComponent<AnimationComponent>();
            ac.xmin = 0;
            ac.numFrames = 1;
            enemy.AddComponent<isShootingComponent>();
        } break;
        case GORDONBOSSAI:{
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<ItemTableComponent>(spriteEnum);
            enemy.AddComponent<BossAIComponent>(GORDON, spawnpoint, 45000, 10000, 2000);
            auto& sec = enemy.GetComponent<StatusEffectComponent>();
            sec.effects[INVULNERABLE] = true;
            sec.endTimes[INVULNERABLE] = 0-1;
            auto& ac = enemy.GetComponent<AnimationComponent>();
            ac.xmin = 0;
            ac.numFrames = 1;
            enemy.AddComponent<isShootingComponent>();
        } break;
        case CUBEGODAI:{
            enemy.AddComponent<BossAIComponent>(CUBEGOD, spawnpoint,0,0,0);
            enemy.AddComponent<ItemTableComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<isShootingComponent>();
        } break;
    }

    return enemy;
}

Factory::Factory(){
    for(int i = 0; i < 12; i++){ // destination positions for partcile velocity calculations
        particleAngles[i] = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(12);
    }
}

Entity Factory::spawnGodLandsSpawner(std::unique_ptr<Registry>& registry, const room& room, int maxMonsters){
    Entity spawner = registry->CreateEntity();
    spawner.AddComponent<EnemySpawnerComponent>(room, maxMonsters);
    return spawner;
}

void Factory::spawnAOEParticles(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, float radius){
    // these particles will need their own system & component for movement & elimintaion
    const SDL_Rect redsquare = {0,8*5,8,8};
    glm::vec2 velocity;
    glm::vec2 spawnPointCenter = {spawnpoint.x + 20, spawnpoint.y + 20};
    auto time = SDL_GetTicks();
    for(int i = 0; i < 12; i++){
        Entity particle = registry->CreateEntity();
        glm::vec2 destPos = {spawnPointCenter.x + 5 * std::cos(particleAngles[i]), spawnPointCenter.y + 5 * std::sin(particleAngles[i])};
        float rotationDegrees = getRotationFromCoordiante(2000, spawnPointCenter.x, spawnPointCenter.y, destPos.x, destPos.y, velocity);
        particle.AddComponent<RidigBodyComponent>(velocity);
        particle.AddComponent<SpriteComponent>(LOFIOBJ, 2,2,redsquare,3,false,false);
        particle.AddComponent<TransformComponent>(glm::vec2(spawnPointCenter.x, spawnPointCenter.y), glm::vec2(5.0,5.0), rotationDegrees);
        float timeToReachDestination = radius / glm::length(velocity) * 1000.0f;
        particle.AddComponent<ParticleComponent>(time + timeToReachDestination);
    }
}

void Factory::populateDungeonWithMonsters(std::unique_ptr<Registry>& registry, std::vector<room>& dungeonRooms, wallTheme dungeonType, int bossRoomId, std::vector<BossIds>& bosses){
    for(const auto& room: dungeonRooms){
        if(room.id == 0){
            continue;
        } else if(room.id == bossRoomId){ // spawn boss in boss room
            glm::vec2 spawnPos = glm::vec2( ((room.x + (room.w / 2)) * 64)-48, ((room.y + (room.h / 2)) * 64)-48);
            switch(dungeonType){
                case CHICKENLAIR:{
                    Entity boss = spawnMonster(registry, spawnPos, BOSSCHICKEN);
                    bosses.push_back({boss.GetId(), boss.GetCreationId()});
                } break;
                case UDL:{
                    Entity boss = spawnMonster(registry, spawnPos, ARCMAGE);          
                    bosses.push_back({boss.GetId(), boss.GetCreationId()});
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
        std::unordered_set<glm::vec2, Vec2Hash> usedSpawnPoints;
        for(const auto& enemySpawn: selectedRoomSpawns){
            int numToSpawn = enemySpawn.modifier * roomQuantifier;
            for(int i = 0; i <= numToSpawn; i++){
                // multiply spawnPos by 64 to convert from tile location to pixel location
                glm::vec2 spawnPos;
                do {
                    spawnPos = {RNG.randomFromRange(minX, maxX) * 64, RNG.randomFromRange(minY, maxY) * 64};
                } while (usedSpawnPoints.find(spawnPos) != usedSpawnPoints.end());
                usedSpawnPoints.insert(spawnPos);
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
    portal.AddComponent<BoxColliderComponent>(PORTALBOX); // same size, its fine
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