#include "factory.h"

Entity Factory::spawnMonster(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum, int parentId){
    Entity enemy = registry->CreateEntity();
    enemy.Group(MONSTER);
    switch(spriteEnum){
        case PENTARACT:{
            enemy.AddComponent<BossAIComponent>(PENTARACT, spawnpoint, 0,0,0);
            enemy.AddComponent<InvisibleBossComponent>();
        } break;
        case ABYSSALSTARPARENT:{
            enemy.AddComponent<SpriteComponent>(LOFICHAR, 8,8,SDL_Rect({0,8*22,8,8}),4,false,false); // invisible sprite 
        } break;
        default:{
            enemy.AddComponent<HPMPComponent>(spriteEnum);
            enemy.AddComponent<SpriteComponent>(spriteEnum);
            enemy.GetComponent<SpriteComponent>().flip = flips[RNG.randomFromRange(0,1)]; // monsters spawns facing random direction
            enemy.AddComponent<BoxColliderComponent>(spriteEnum);
            enemy.AddComponent<CollisionFlagComponent>();
            enemy.AddComponent<StatusEffectComponent>();
            enemy.AddComponent<SpeedStatComponent>(spriteEnum, RNG); // stationary enemies still have their speedstatcomp taken when hit by quivers
            enemy.AddComponent<DistanceToPlayerComponent>();
        } break;
    }

    // very few monsters will requrie non-default (6.0) scaling
    switch(spriteEnum){
        case GORDON2:
        case AMDUSCIAS:
        case ASTAROTH:
        case ABIGOR:
        case ABYSSTOWER:
        case BROWNSLIMELARGE:
        case BLACKSLIMELARGE:
        case REAPER:
        case GRANDSPHINX:
        case ORANGECUBE:{ 
            enemy.AddComponent<TransformComponent>(spawnpoint, glm::vec2(8.0));
        } break;
        case YELLOWCUBE:{
            enemy.AddComponent<TransformComponent>(spawnpoint, glm::vec2(7.0));
        } break;
        case BLACKSLIMESMALL:
        case BROWNSLIMESMALL:
        case CYANCUBE: { 
            enemy.AddComponent<TransformComponent>(spawnpoint, glm::vec2(5.0));
        } break;
        case MALPHASBABY:
        case TRIPDEMON:{
            enemy.AddComponent<TransformComponent>(spawnpoint, glm::vec2(4.0));
        } break;
        default:{ 
            enemy.AddComponent<TransformComponent>(spawnpoint); // default = 6.0
        } break;
    }

    if(healsOther.find(spriteEnum) != healsOther.end()){
        int interval, amount;
        switch(spriteEnum){
            case TRIPDEMON:{
                interval = RNG.randomFromRange(2500,5000);
                amount = RNG.randomFromRange(100,500);
            } break;
            case CRUSADER4:{
                interval = RNG.randomFromRange(1000,2000);
                amount = RNG.randomFromRange(50,200);
            } break;
            case MYSTERIOUSCRYSTAL:
            case REAPER:{
                interval = 1000;
                amount = 50;
            } break;
            case AMDUSCIAS:
            case ASTAROTH:
            case ABIGOR:{
                interval = 1000;
                amount = 200;
            } break;
            case GORDON2:{
                interval = 1000;
                amount = 1500;
            } break;
        } 
        if(parentId == -1){
            enemy.AddComponent<HealOtherComponent>(enemy.GetId(), registry->GetCreationIdFromEntityId(enemy.GetId()), interval, amount);
        } else {
            enemy.AddComponent<HealOtherComponent>(parentId, registry->GetCreationIdFromEntityId(parentId), interval, amount);    
        }
    }

    if(towers.find(spriteEnum) != towers.end()){ // towers dont flip their sprites, ever
        enemy.AddComponent<TowerComponent>();
    }

    if(hasDeathAction.find(spriteEnum) != hasDeathAction.end()){
        enemy.AddComponent<DeathActionComponent>(spriteEnum);
    }

    // if this monster has a secondary projectile
    if(spriteToSPECRepeatFreq.find(spriteEnum) != spriteToSPECRepeatFreq.end()){
        auto data = spriteToSPECRepeatFreq.at(spriteEnum);
        auto rf = data.RFmin * RNG.randomFromRange(1.0f, data.RFmaxmod);
        enemy.AddComponent<SecondaryProjectileComponent>(rf);
    }

    // if this monster must be grouped with a monster sub-group
    if(spriteToMonsterSubGroups.find(spriteEnum) != spriteToMonsterSubGroups.end()){
        monsterSubGroups msg = spriteToMonsterSubGroups.at(spriteEnum);
        enemy.monsterSubGroup(msg);
    }

    // if this monster should be able to spawn minions
    if(spriteToMinionSpawnerData.find(spriteEnum) != spriteToMinionSpawnerData.end()){
        const auto& msd = spriteToMinionSpawnerData.at(spriteEnum);
        enemy.AddComponent<MinionSpawnerComponent>(msd.minions, msd.maxMinions, msd.respawnInterval, msd.spawnOnlyOnce);
    }

    // if this monster should be able to drop loot
    if(spriteEnumToItemTableComponentData.find(spriteEnum) != spriteEnumToItemTableComponentData.end()){
        enemy.AddComponent<ItemTableComponent>(spriteEnum);
    }

    enemyCategory enemyCat = spriteToEnemyCategory.at(spriteEnum);
    switch(enemyCat){
        case INVISIBLEENEMY:{
            return enemy;
        } break;
        case RANDOMCHASEMINION:{
            enemy.AddComponent<isShootingComponent>();
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RandomChaseMinionComponent>(RNG.randomFromRange(3000,10000));
            enemy.AddComponent<MinionComponent>(parentId, registry->GetCreationIdFromEntityId(parentId), spriteEnum);
            enemy.AddComponent<RidigBodyComponent>();
            switch(spriteEnum){
                case TRIPDEMON:
                case REAPER:
                case HORRIDREAPER1:
                case HORRIDREAPER2:{
                    enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
                    enemy.AddComponent<AnimationComponent>(spriteEnum);
                } break;
            }
        } break;
        case PASSIVE:{
            enemy.AddComponent<PassiveAIComponent>(1000);
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimationComponent>(1,3);
        } break;
        case NEUTRAL:{
            enemy.AddComponent<isShootingComponent>();
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<NeutralAIComponent>(spriteEnum);
        } break;
        case SHOOTORBITMINION:{
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<MinionComponent>(parentId, registry->GetCreationIdFromEntityId(parentId));
            const auto& orbitData = spriteToMinionTravelDistanceData.at(spriteEnum);
            auto distance = orbitData.distance * RNG.randomFromRange(1.0f, orbitData.distanceModMax);
            enemy.AddComponent<OrbitalMovementComponent>(distance, RNG.randomBool());
            enemy.AddComponent<isShootingComponent>();
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<OrbitShootMinionComponent>(RNG.randomFromRange(150,550));
        } break;
        case ORBITMINION:{
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<MinionComponent>(parentId, registry->GetCreationIdFromEntityId(parentId));
            const auto& orbitData = spriteToMinionTravelDistanceData.at(spriteEnum);
            auto distance = orbitData.distance * RNG.randomFromRange(1.0f, orbitData.distanceModMax);
            enemy.AddComponent<OrbitalMovementComponent>(distance, RNG.randomBool());
            enemy.AddComponent<OrbitMinionComponent>();
        } break;
        case STANDSHOOTMINION:{
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<MinionComponent>(parentId, registry->GetCreationIdFromEntityId(parentId));
            const auto& data = spriteToMinionTravelDistanceData.at(spriteEnum);
            auto distance = data.distance * RNG.randomFromRange(1.0f, data.distanceModMax);
            enemy.AddComponent<StandShootMinionComponent>(RNG.randomFromRange(150,550), distance);
            enemy.AddComponent<isShootingComponent>();
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
        } break;
        case KEY:{ // keys are a legacy monster category
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<RidigBodyComponent>();
        } break;
        case ASC:{ // animated shooting chase category
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<AnimatedChaseAIComponent>(spriteEnum, RNG);
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<isShootingComponent>();
        } break;
        case AS:{ // animated shooting category
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<AnimatedNeutralAIComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<isShootingComponent>();
        } break;
        case SC:{ // shooting chase category
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<ChaseAIComponent>(spriteEnum, RNG);
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
            enemy.AddComponent<BossAIComponent>(BOSSCHICKEN, spawnpoint, 2000, 200, 700);
            enemy.GetComponent<TransformComponent>().position = enemy.GetComponent<BossAIComponent>().phaseOnePositions[0]; // for chicken circle
            enemy.AddComponent<isShootingComponent>();
        } break;
        case ARCMAGEBOSSAI:{
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
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
            enemy.AddComponent<BossAIComponent>(GORDON, spawnpoint, 35000, 10000, 2000);
            auto& sec = enemy.GetComponent<StatusEffectComponent>();
            sec.effects[INVULNERABLE] = true;
            sec.endTimes[INVULNERABLE] = 0-1;
            auto& ac = enemy.GetComponent<AnimationComponent>();
            ac.xmin = 0;
            ac.numFrames = 1;
            enemy.AddComponent<isShootingComponent>();
        } break;
        case CUBEGODAI:{ // skull shrines are also cube gods
            enemy.AddComponent<BossAIComponent>(CUBEGOD, spawnpoint,0,0,0);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<isShootingComponent>();
        } break;
        case CRYSTALPRISONERAI:{
            enemy.AddComponent<BossAIComponent>(CRYSTALPRISONER, spawnpoint,0,0,0);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<isShootingComponent>();
        } break;
        case GRANDSPHINXAI:{
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<BossAIComponent>(GRANDSPHINX, spawnpoint, 0, 0, 0);
            enemy.AddComponent<isShootingComponent>();
        } break;
        case ASTAROTHAI:{
            enemy.AddComponent<OrbitalMovementComponent>(400.0f, false, 2.09f);
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<BossAIComponent>(spriteEnum, spawnpoint, 0, 0, 0);
            enemy.AddComponent<isShootingComponent>();
            enemy.GetComponent<SpeedStatComponent>().activespeed = spriteEnumToStatData.at(spriteEnum).activespeed; // override random small change
            auto& sec = enemy.GetComponent<StatusEffectComponent>();
            sec.effects[INVULNERABLE] = true;
            sec.endTimes[INVULNERABLE] = 0-1;
            auto& ac = enemy.GetComponent<AnimationComponent>();
            ac.xmin = 0;
            ac.numFrames = 1;
        } break;
        case AMDUSCIASAI:{
            enemy.AddComponent<OrbitalMovementComponent>(400.0f, false, 2.09f * 2.0f);
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<BossAIComponent>(spriteEnum, spawnpoint, 0, 0, 0);
            enemy.AddComponent<isShootingComponent>();
            enemy.GetComponent<SpeedStatComponent>().activespeed = spriteEnumToStatData.at(spriteEnum).activespeed; // override random small change
            auto& sec = enemy.GetComponent<StatusEffectComponent>();
            sec.effects[INVULNERABLE] = true;
            sec.endTimes[INVULNERABLE] = 0-1;
            auto& ac = enemy.GetComponent<AnimationComponent>();
            ac.xmin = 0;
            ac.numFrames = 1;
        } break;
        case ABIGORAI:{
            enemy.AddComponent<OrbitalMovementComponent>(400.0f, false, 2.09f * 3.0f);
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<BossAIComponent>(spriteEnum, spawnpoint, 0, 0, 0);
            enemy.AddComponent<isShootingComponent>();
            enemy.GetComponent<SpeedStatComponent>().activespeed = spriteEnumToStatData.at(spriteEnum).activespeed; // override random small change
            auto& sec = enemy.GetComponent<StatusEffectComponent>();
            sec.effects[INVULNERABLE] = true;
            sec.endTimes[INVULNERABLE] = 0-1;
            auto& ac = enemy.GetComponent<AnimationComponent>();
            ac.xmin = 0;
            ac.numFrames = 1;
        } break;
        case GORDONBOSSAI2:{
            enemy.AddComponent<AnimationComponent>(spriteEnum);
            enemy.AddComponent<ProjectileEmitterComponent>(spriteEnum, enemy.GetId());
            enemy.AddComponent<RidigBodyComponent>();
            enemy.AddComponent<AnimatedShootingComponent>(spriteEnum);
            enemy.AddComponent<BossAIComponent>(spriteEnum, spawnpoint, 0, 0, 0);
            auto& sec = enemy.GetComponent<StatusEffectComponent>();
            sec.effects[INVULNERABLE] = true;
            sec.endTimes[INVULNERABLE] = 0-1;
            auto& ac = enemy.GetComponent<AnimationComponent>();
            ac.xmin = 0;
            ac.numFrames = 1;
            enemy.AddComponent<isShootingComponent>();
            enemy.AddComponent<OrbitalMovementComponent>(900.0f, false, spawnpoint);
        } break;
    }

    return enemy;
}

void Factory::spawnDecoration(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum){
    Entity decoration = registry->CreateEntity();
    std::vector<SDL_RendererFlip> flipsOne = {SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL};
    std::vector<SDL_RendererFlip> flipsTwo = {SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL};
    switch(spriteEnum){
        case CRACKEDWHITEEGG:{
            decoration.AddComponent<SpriteComponent>(LOFICHAR,8,8,4,8*6,8*14,false);
            decoration.AddComponent<TransformComponent>(spawnpoint);
        } break;
        case CROSSGRAVE:{
            decoration.AddComponent<SpriteComponent>(LOFIOBJ,8,8,4,8*4,8*1,false);
            decoration.AddComponent<TransformComponent>(spawnpoint);
        } break;
        case FLOORBLOOD:{
            auto xpos = RNG.randomFromRange(3,5);
            decoration.AddComponent<SpriteComponent>(LOFIOBJ,8,8,1,xpos*8,8*5,false,RNG.randomFromVector(flipsTwo));
            decoration.AddComponent<TransformComponent>(spawnpoint);
        } break;
        case FLOORSKULL:{
            decoration.AddComponent<SpriteComponent>(LOFIOBJ,8,8,2,0,8,false,RNG.randomFromVector(flipsOne));
            decoration.AddComponent<TransformComponent>(spawnpoint);
        } break;
        case CRACKEDDRAGONEGG:{
            decoration.AddComponent<SpriteComponent>(LOFICHAR,8,8,4,8*8,8*14,false);
            decoration.AddComponent<TransformComponent>(spawnpoint);
        } break;
    }
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

void Factory::spawnAOEParticles(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, float radius, colors color){
    SDL_Rect square = {0,8*5,8,8};
    switch(color){
        case RED:{
            square = {0,8*5,8,8};
        } break;
        case BLACK:{
            square = {8*2,8*4,8,8};   
        } break;
    }
    glm::vec2 velocity;
    glm::vec2 spawnPointCenter = {spawnpoint.x + 20, spawnpoint.y + 20};
    auto time = SDL_GetTicks();
    for(int i = 0; i < 12; i++){
        Entity particle = registry->CreateEntity();
        glm::vec2 destPos = {spawnPointCenter.x + 5 * std::cos(particleAngles[i]), spawnPointCenter.y + 5 * std::sin(particleAngles[i])};
        float rotationDegrees = getRotationFromCoordiante(2000, spawnPointCenter.x, spawnPointCenter.y, destPos.x, destPos.y, velocity);
        particle.AddComponent<RidigBodyComponent>(velocity);
        particle.AddComponent<SpriteComponent>(LOFIOBJ, 2,2,square,3,false,false); // ground particle should be below monsters, so zaxis = 3
        particle.AddComponent<TransformComponent>(glm::vec2(spawnPointCenter.x, spawnPointCenter.y), glm::vec2(5.0,5.0), rotationDegrees);
        float timeToReachDestination = radius / glm::length(velocity) * 1000.0f;
        particle.AddComponent<ParticleComponent>(time + timeToReachDestination);
    }
}

void Factory::spawnNecromancerParticles(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, float radius){
    const SDL_Rect redsquare = {0,8*5,8,8};
    const SDL_Rect purplesquare = {8*2,8*5,8,8};
    glm::vec2 velocity;
    auto time = SDL_GetTicks();
    for(int i = 0; i < 12; i++){
        Entity particle = registry->CreateEntity();
        glm::vec2 destPos = {spawnpoint.x + 5 * std::cos(particleAngles[i]), spawnpoint.y + 5 * std::sin(particleAngles[i])};
        float rotationDegrees = getRotationFromCoordiante(1000, spawnpoint.x, spawnpoint.y, destPos.x, destPos.y, velocity);
        particle.AddComponent<RidigBodyComponent>(velocity);
        if(i%2==0){
            particle.AddComponent<SpriteComponent>(LOFIOBJ, 2,2,purplesquare,3,false,false); 
        } else {
            particle.AddComponent<SpriteComponent>(LOFIOBJ, 2,2,redsquare,3,false,false);     
        }
        particle.AddComponent<TransformComponent>(glm::vec2(spawnpoint.x, spawnpoint.y) + RNG.randomFromRange(-5.0f,5.0f), glm::vec2(6.0,6.0), rotationDegrees);
        float timeToReachDestination = radius / glm::length(velocity) * 1000.0f;
        particle.AddComponent<ParticleComponent>(time + timeToReachDestination);
    }
}

void Factory::spawnLinearParticle(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const glm::vec2 endpoint, float speed){
    const SDL_Rect redsquare = {0,8*5,8,8};
    glm::vec2 velocity;
    auto time = SDL_GetTicks();
    Entity particle = registry->CreateEntity();
    float scale = RNG.randomFromRange(5.0,8.0);
    float rotationDegrees = getRotationFromCoordiante(speed, spawnpoint.x, spawnpoint.y, endpoint.x, endpoint.y, velocity);
    particle.AddComponent<RidigBodyComponent>(velocity);
    particle.AddComponent<SpriteComponent>(LOFIOBJ, 2,2,redsquare,5,false,false); // these are floating particles, z-axis = 5?
    particle.AddComponent<TransformComponent>(glm::vec2(spawnpoint.x, spawnpoint.y), glm::vec2(scale), rotationDegrees);
    float timeToReachDestination = glm::distance(spawnpoint, endpoint) / glm::length(velocity) * 1000.0f;
    particle.AddComponent<ParticleComponent>(time + timeToReachDestination);
}

void Factory::spawnHealOtherPartiles(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const glm::vec2& endpoint){
    constexpr int numParticles = 10;
    const SDL_Rect pinksquare = {8*3,8*4,8,8};
    constexpr int spriteDimension = 2;
    glm::vec2 step = (endpoint - spawnpoint) / static_cast<float>(numParticles - 1);
    Entity particle;
    for(int i = 0; i < numParticles; i++){
        particle = registry->CreateEntity();
        particle.AddComponent<RidigBodyComponent>();
        particle.AddComponent<SpriteComponent>(LOFIOBJ, spriteDimension,spriteDimension,pinksquare,5,false,false); 
        particle.AddComponent<TransformComponent>(spawnpoint + (step * static_cast<float>(i)) - ((10.0f * spriteDimension)/2), glm::vec2(10.0f));
        particle.AddComponent<ParticleComponent>(0-1, true);
    }
}

// spawns 10 particles along path of travel
Entity Factory::spawnScepterParticles(Entity player, std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const glm::vec2& endpoint, int damage){
    constexpr int numParticles = 10;
    const SDL_Rect pinksquare = {8*1,8*5,8,8};
    constexpr float startingScale = 10.0f;
    constexpr int spriteDimension = 2;
    auto time = SDL_GetTicks();
    std::vector<glm::vec2> points(numParticles);
    glm::vec2 step = (endpoint - spawnpoint) / static_cast<float>(numParticles - 1);
    Entity particle;
    for(int i = 0; i < numParticles; i++){
        float currentScale = startingScale * std::exp(std::log(4.0) / 10.0 * i); // exponential scale from 10 to 26 for scale value
        particle = registry->CreateEntity();
        particle.AddComponent<RidigBodyComponent>();
        particle.AddComponent<SpriteComponent>(LOFIOBJ, spriteDimension,spriteDimension,pinksquare,5,false,false); 
        particle.AddComponent<TransformComponent>(spawnpoint + (step * static_cast<float>(i)) - ((currentScale * spriteDimension)/2), glm::vec2(currentScale));
        particle.AddComponent<ParticleComponent>(0-1, true);
    }
    particle.AddComponent<ProjectileComponent>(damage,INT_MAX,false,player, 4,NONESPRITE); // bogus projectile component 
    return particle;
}

void Factory::spawnScepterFailParticles(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const glm::vec2& endpoint){
    constexpr int numParticles = 20;
    // constexpr float angleOffset = glm::radians(10.0f); // 10 degrees in radians
    constexpr float distance = 500.0f; // static distance of 1000 units
    constexpr float angleOffset = 15.0f;
    const SDL_Rect pinksquare = {8*1,8*5,8,8};
    constexpr int spriteDimension = 2;
    constexpr glm::vec2 scale(8.0);
    auto time = SDL_GetTicks();
    float angleDegrees = std::atan2(endpoint.y - spawnpoint.y, endpoint.x - spawnpoint.x) * (180.0 / M_PI);   
    for(int i = -1; i <= 1; i++){
        float angleRadians = glm::radians(angleDegrees + i * angleOffset);
        glm::vec2 destination = spawnpoint + glm::vec2({distance * std::cos(angleRadians), distance * std::sin(angleRadians)});
        glm::vec2 step = (destination - spawnpoint) / static_cast<float>(numParticles - 1);
        for(int j = 0; j < numParticles; j++){
            glm::vec2 position = spawnpoint + (step * static_cast<float>(j)) - ((scale.x * spriteDimension)/2) + RNG.randomFromRange(-10.0f, 10.0f);
            Entity particle = registry->CreateEntity();
            particle.AddComponent<RidigBodyComponent>();
            particle.AddComponent<SpriteComponent>(LOFIOBJ, spriteDimension,spriteDimension,pinksquare,3,false,false); 
            particle.AddComponent<TransformComponent>(position, scale);
            particle.AddComponent<ParticleComponent>(0-1, true);
        }
    }
}

void Factory::populateDungeonWithMonsters(std::unique_ptr<Registry>& registry, std::vector<room>& dungeonRooms, wallTheme dungeonType, int bossRoomId, std::vector<BossIds>& bosses, std::map<cardinalDirection, room>& cardinalRooms){
    bool spawnedTroom = false;
    int denominatorTRoom;
    std::unordered_set<int> cardinalRoomIds;
    switch(dungeonType){
        case ABYSS:{
            denominatorTRoom = 400;
            for(auto& pair: cardinalRooms){
                cardinalRoomIds.insert(pair.second.id);
            }
        } break;
        default:{
            denominatorTRoom = 250;
        } break;
    }
    for(const auto& room: dungeonRooms){
        glm::vec2 roomCenter = glm::vec2(((room.x + (room.w / 2)) * 64), ((room.y + (room.h / 2)) * 64));
        if(room.id == 0){ // spawn room
            if(dungeonType == ABYSS){ // bosses spawn in spawn room of abyss
                // std::cout << "spawning bosses at " << roomCenter.x << ", " << roomCenter.y << '\n'; 
                constexpr float distance = 350.0f;
                constexpr std::array<sprites, 3> bossdemons = {ASTAROTH, AMDUSCIAS, ABIGOR};
                std::vector<Entity> abyssBosses; // will be {astaroth, amduscias, abigor}
                float angleRadians = 2.09f;
                for(int i = 0; i < 3; i++){
                    glm::vec2 spawnPos((roomCenter.x + distance * glm::cos(angleRadians)) - 124.0f, (roomCenter.y + distance * glm::sin(angleRadians)) - 64.0f);
                    Entity boss = spawnMonster(registry, spawnPos, bossdemons[i]);
                    abyssBosses.push_back(boss);
                    bosses.push_back({boss.GetId(), boss.GetCreationId()});
                    if(i == 2){ // abigor spawns to the right of player, flip his sprite
                        auto& flip = boss.GetComponent<SpriteComponent>().flip;
                        flip = SDL_FLIP_HORIZONTAL;
                    }
                    angleRadians += 2.09f;
                }
                auto& astaroth = abyssBosses[0];
                auto& amduscias = abyssBosses[1];
                auto& abigor = abyssBosses[2];
                auto& astarothHOC = astaroth.GetComponent<HealOtherComponent>();
                auto& amdusciasHOC = amduscias.GetComponent<HealOtherComponent>();
                auto& abigorHOC = abigor.GetComponent<HealOtherComponent>();
                astarothHOC.beneficiaryId = amduscias.GetId(); // astaroth heals amduscias
                astarothHOC.beneficiaryCreationId = amduscias.GetCreationId();
                astarothHOC.beneficiaryIsDead = true;
                amdusciasHOC.beneficiaryId = abigor.GetId(); // amduscias heals abigor
                amdusciasHOC.beneficiaryCreationId = abigor.GetCreationId();
                amdusciasHOC.beneficiaryIsDead = true;
                abigorHOC.beneficiaryId = astaroth.GetId(); // abigor heals astaroth
                abigorHOC.beneficiaryCreationId = astaroth.GetCreationId();
                abigorHOC.beneficiaryIsDead = true;
                auto& astarothBAI = astaroth.GetComponent<BossAIComponent>();
                astarothBAI.idKeyOne = amduscias.GetId();
                astarothBAI.cIdKeyOne = amduscias.GetCreationId();
                astarothBAI.idKeyTwo = abigor.GetId();
                astarothBAI.cIdKeyTwo = abigor.GetCreationId();
                auto& amdusciasBAI = amduscias.GetComponent<BossAIComponent>();
                amdusciasBAI.idKeyOne = abigor.GetId();
                amdusciasBAI.cIdKeyOne = abigor.GetCreationId();
                amdusciasBAI.idKeyTwo = astaroth.GetId();
                amdusciasBAI.cIdKeyTwo = astaroth.GetCreationId();
                auto& abigorBAI = abigor.GetComponent<BossAIComponent>();
                abigorBAI.idKeyOne = astaroth.GetId();
                abigorBAI.cIdKeyOne = astaroth.GetCreationId();
                abigorBAI.idKeyTwo = amduscias.GetId();
                abigorBAI.cIdKeyTwo = amduscias.GetCreationId();

            }
            continue;
        } else if(room.id == bossRoomId && dungeonType != ABYSS){ // spawn boss in boss room
            switch(dungeonType){
                case CHICKENLAIR:{
                    Entity boss = spawnMonster(registry, roomCenter - 48.0f, BOSSCHICKEN);
                    bosses.push_back({boss.GetId(), boss.GetCreationId()});
                } break;
                case UDL:{
                    Entity boss = spawnMonster(registry, roomCenter - 48.0f, ARCMAGE);          
                    bosses.push_back({boss.GetId(), boss.GetCreationId()});
                } break;
            }
            continue;
        } else if(dungeonType == ABYSS && cardinalRoomIds.find(room.id) != cardinalRoomIds.end()){ // corner room for towers
            Entity boss = spawnMonster(registry, roomCenter - 64.0f, ABYSSTOWER);
            bosses.push_back({boss.GetId(), boss.GetCreationId()});
            continue;
        }
        // else, populate this room with monsters!
        int minX = room.x + 2; // these values are in tiles
        int minY = room.y + 2;
        int maxX = room.x + room.w - 2;
        int maxY = room.y + room.h - 2;
        const auto& possibleRoomSpawns = wallThemeToMonsterSpawns.at(dungeonType); // the table that had possible room spawns; ex tiny red and white chickens together
        const auto& selectedRoomSpawns = possibleRoomSpawns[RNG.randomFromRange(0, possibleRoomSpawns.size()-1)]; // select random from wallThemeToMonsterSpawns
        int roomQuantifier = (room.w + room.h) / 4; // ex 10x10 room will spawn 10 monsters per spawn type in selectedRoomSpawns
        if(!spawnedTroom && RNG.randomFromRange(1, denominatorTRoom) == 1){ // each room has 1/250 chance to be a treasure room; around 10% for a dugeon w/ 30 rooms to have a treasure room
            spawnTreasureRoomChest(registry, roomCenter - 24.0f, dungeonType);
            spawnedTroom = true;
        } else {
            for(const auto& enemySpawn: selectedRoomSpawns){
                std::unordered_set<glm::vec2, Vec2Hash> usedSpawnPoints;
                if(enemySpawn.modifier == 0.0){ // flag to spawn just one monster at room center. ex: shade, cultist
                    spawnMonster(registry, roomCenter - 48.0f, enemySpawn.monster);
                } else {
                    int numToSpawn = enemySpawn.modifier * roomQuantifier;
                    for(int i = 0; i <= numToSpawn; i++){
                        // multiply spawnPos by 64 to convert from tile location to pixel location
                        glm::vec2 spawnPos;
                        do {
                            spawnPos = {(RNG.randomFromRange(minX, maxX) * 64) - 32, (RNG.randomFromRange(minY, maxY) * 64) - 32};
                        } while (usedSpawnPoints.find(spawnPos) != usedSpawnPoints.end());
                        usedSpawnPoints.insert(spawnPos); // stores tile selected, ignores random offset that is about to be applied
                        spawnPos.x += RNG.randomFromRange(-8.0f,8.0f);
                        spawnPos.y += RNG.randomFromRange(-8.0f,8.0f);
                        spawnMonster(registry, spawnPos, enemySpawn.monster);
                    }
                }
            }
        }
    }
    registry->Update();
}

void Factory::spawnTreasureRoomChest(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, wallTheme area){
    Entity chest = registry->CreateEntity();
    chest.Group(MONSTER);
    switch(area){
        case CHICKENLAIR:{
            chest.AddComponent<ItemTableComponent>(BOSSCHICKEN);
            for(int i = 0; i < 5; i++){
                spawnMonster(registry, spawnpoint, DUCKLING, chest.GetId());
            } break;
        } break;
        case UDL:{
            chest.AddComponent<ItemTableComponent>(ARCMAGE);
            for(int i = 0; i < 6; i++){
                float randomAngle = glm::linearRand(0.0f, 6.2831855f);
                float offsetX = RNG.randomFromRange(50,75) * glm::cos(randomAngle); 
                float offsetY = RNG.randomFromRange(50,75) * glm::sin(randomAngle);
                glm::vec2 realSpawnPos = {(spawnpoint.x + offsetX), (spawnpoint.y + offsetY)};
                switch(i){
                    case 1:{
                        spawnDecoration(registry, realSpawnPos, CROSSGRAVE);
                    } break;
                    case 2:{
                        spawnDecoration(registry, realSpawnPos, FLOORSKULL);
                    } break;
                    default:{
                        spawnDecoration(registry, realSpawnPos, FLOORBLOOD);
                    }
                }

            }
        } break;
        case ABYSS:{
            constexpr std::array<sprites, 3> abyssbosses = {ABIGOR, AMDUSCIAS, ASTAROTH};
            int bossIndex = RNG.randomFromRange(0, abyssbosses.size()-1);
            chest.AddComponent<ItemTableComponent>(abyssbosses[bossIndex]);
            constexpr std::array<sprites, 2> greenbrutes = {GREENBRUTE0, GREENBRUTE1};
            for(int i = 0; i < 6; i++){
                int bruteIndex = RNG.randomFromRange(0,1);
                spawnMonster(registry, spawnpoint, greenbrutes[bruteIndex], chest.GetId());
            }
        } break;
    }
    chest.AddComponent<TransformComponent>(spawnpoint);
    chest.AddComponent<HPMPComponent>(POTCHEST);
    chest.AddComponent<SpriteComponent>(POTCHEST);
    chest.GetComponent<SpriteComponent>().flip = flips[RNG.randomFromRange(0,1)]; // monsters spawns facing random direction
    chest.AddComponent<BoxColliderComponent>(POTCHEST);
    chest.AddComponent<CollisionFlagComponent>();
    chest.AddComponent<StatusEffectComponent>();
    chest.AddComponent<SpeedStatComponent>(POTCHEST, RNG); // stationary enemies still have their speedstatcomp taken when hit by quivers
    chest.AddComponent<DistanceToPlayerComponent>();
    chest.AddComponent<TowerComponent>();
    chest.AddComponent<isShootingComponent>();
    chest.AddComponent<ProjectileEmitterComponent>(POTCHEST, chest.GetId());
    chest.AddComponent<NeutralAIComponent>(POTCHEST);

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

// overloaded version that generates lootbag with spriteEnum and position rather than a monster reference; can be called for any monster at any time
void Factory::createLootAtDeath(sprites spriteEnum, const glm::vec2& spawnPos, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore){
    auto tables = spriteEnumToItemTableComponentData.at(spriteEnum).dropTable;
    Entity lootbag = registry->CreateEntity();
    lootbag.Group(LOOTBAGGROUP);
    lootbag.AddComponent<LootBagComponent>();
    auto& lbc = lootbag.GetComponent<LootBagComponent>();
    int roll;
    sprites bagsprite = BROWNLOOTBAG;
    for(const auto& table: tables){
        roll = RNG.randomFromRange(1,100);
        if(roll <= table.first){ // if value is lower than table rate, table is hit so random item from it
            const auto& item = table.second[RNG.randomFromRange(0,table.second.size()-1)];
            createItemInBag(registry, item, lootbag);
            itemEnumToLootBagSpriteEnum.at(item) > bagsprite ? bagsprite = itemEnumToLootBagSpriteEnum.at(item) : bagsprite = bagsprite;
        }
    } if(lbc.contents.size() > 0){
        lootbag.AddComponent<TransformComponent>(spawnPos, glm::vec2(5.0,5.0));
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
    for(int i = 0; i <= 8; i++){
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

void Factory::spawnAdminLootInNexus(std::unique_ptr<Registry>& registry){
    constexpr std::array<items, 8> warriorKnight = {T10SWORD, T4HELM, T10HEAVYARMOR, T4HPRING, T10SWORD, T4SHIELD, T10HEAVYARMOR, T4HPRING};
    constexpr std::array<items, 8> priestNecro = {T10WAND, T4TOME, T10ROBE, T4HPRING, T10STAFF, T4SKULL, T10ROBE, T4HPRING};
    constexpr std::array<items, 8> sorcPaladin = {T10WAND, T4SCEPTER, T10ROBE, T4HPRING, T10SWORD, T4SEAL, T10HEAVYARMOR, T4HPRING};
    constexpr std::array<items, 8> wizardArcher = {T10STAFF, T4SPELL, T10ROBE, T4HPRING, T10BOW, T4QUIVER, T10LIGHTARMOR, T4HPRING};
    constexpr std::array<items, 8> rogue = {T10DAGGER, T4CLOAK, T10HEAVYARMOR, T4HPRING, T14DAGGER, T14SWORD, T14STAFF, TRIPLESCEPTER};
    constexpr std::array<items, 8> extra = {ADMINCROWN, CHRONUSDIRK, ADMINSWORD, ADMINWAND, OGMUR, CURLYWHIRLYSPELL, GORDONINCANTATION, OREO};
    constexpr std::array<items, 8> extra2 = {T5HPRING, T5MPRING, T5ATTRING, T5DEFRING, T5DEXRING, CHICKENSWORD, DEMONBLADE, T14BOW};
    std::vector<std::array<items, 8>> bags = {warriorKnight, priestNecro, sorcPaladin, wizardArcher, rogue, extra, extra2};
    glm::vec2 bagSpawn(750,1350);
    for(auto& bag: bags){
        bagSpawn.y -= 64.0f;    
        Entity lootbag = creatLootBag(registry, bagSpawn, BROWNLOOTBAG);
        for(auto& item: bag){
            createItemInBag(registry, item, lootbag);
        }
    }
    for(int i = 0; i < 3; i++){
        bagSpawn.y -= 64.0f; 
        spawnMonster(registry, bagSpawn, POTCHEST);
    }
}

void Factory::spawnNumberEntity(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, std::string&& number){
    Entity entity = registry->CreateEntity();
    entity.AddComponent<TextLabelComponent>(number, "namefont", grey, true, 0,0,0);
    entity.AddComponent<TransformComponent>(spawnpoint);
}