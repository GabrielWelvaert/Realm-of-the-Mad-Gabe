#include "ArtificialIntelligenceSystem.h"

inline float chasePlayer(const glm::vec2& origin, const glm::vec2& dest, glm::vec2& monsterVelocity){
    float angleRadians = std::atan2(dest.y - origin.y, dest.x - origin.x);   
    monsterVelocity.x = std::cos(angleRadians);
    monsterVelocity.y = std::sin(angleRadians); // x and y will be multiplied by speed in movementSystem
    return monsterVelocity.x;
}

inline float getDistanceToPlayer(const glm::vec2& origin, const glm::vec2& destination) {
    return sqrt(pow(origin.x - destination.x, 2) + pow(origin.y - destination.y, 2));
}

PassiveAISystem::PassiveAISystem(){
    RequireComponent<PassiveAIComponent>();
}

void PassiveAISystem::Update(const glm::vec2& playerPos){return;}

ChaseAISystem::ChaseAISystem(){
    RequireComponent<ChaseAIComponent>();
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<TransformComponent>();
}

void ChaseAISystem::Update(const glm::vec2& playerPos){
    for(auto& entity: GetSystemEntities()){
        const auto& position = entity.GetComponent<TransformComponent>().position;
        float distanceToPlayer = getDistanceToPlayer(position, playerPos);
        if(distanceToPlayer > 1000){continue;} // hopefully already had its stuff turned off! 
        auto& aidata = entity.GetComponent<ChaseAIComponent>();
        auto& pec = entity.GetComponent<ProjectileEmitterComponent>();
        auto& velocity = entity.GetComponent<RidigBodyComponent>().velocity;
        auto& flip = entity.GetComponent<SpriteComponent>().flip;
        if(distanceToPlayer <= aidata.detectRange){
            if(distanceToPlayer <= aidata.engageRange){
                if(distanceToPlayer <= aidata.maxDistance){ // shoot dont chase 
                    velocity.x = 0;
                    velocity.y = 0;
                } else { // shoot, chase 
                    if(chasePlayer(position, playerPos, velocity) < 0){ // facing left
                        flip = SDL_FLIP_HORIZONTAL;
                    } else { // facing right
                        flip = SDL_FLIP_NONE;
                    }  
                    if(pec.isShooting == false){
                        pec.isShooting = true;
                    } 
                }
            } else { // chase, dont shoot 
                if(chasePlayer(position, playerPos, velocity) < 0){ // facing left
                    flip = SDL_FLIP_HORIZONTAL;
                } else { // facing right
                    flip = SDL_FLIP_NONE;
                }
                pec.isShooting = false;
            }

        } else { // dont shoot or chase; out of detetctRange
            pec.isShooting = false;
            velocity.x = 0;
            velocity.y = 0; 
        }
        
    }
}

NeutralAISystem::NeutralAISystem(){
    RequireComponent<NeutralAIComponent>();
    RequireComponent<ProjectileEmitterComponent>();
}

void NeutralAISystem::Update(const glm::vec2& playerPos){
    for(auto& entity: GetSystemEntities()){
        const auto& position = entity.GetComponent<TransformComponent>().position;
        float distanceToPlayer = getDistanceToPlayer(position, playerPos);
        if(distanceToPlayer > 1000){continue;} // hopefully already had its stuff turned off! 
        auto& aidata = entity.GetComponent<NeutralAIComponent>();
        auto& pec = entity.GetComponent<ProjectileEmitterComponent>();
        auto& flip = entity.GetComponent<SpriteComponent>().flip;
        if(distanceToPlayer <= aidata.engageRange){ // shoot, stand 
            pec.isShooting = true;
            playerPos.x < position.x ? flip = SDL_FLIP_HORIZONTAL : flip = SDL_FLIP_NONE;
        } else { // dont shoot; stand 
            pec.isShooting = false;
            playerPos.x < position.x ? flip = SDL_FLIP_HORIZONTAL : flip = SDL_FLIP_NONE;
        }
        
    }
}

TrapAISystem::TrapAISystem(){
    RequireComponent<TrapAIComponent>();
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<HPMPComponent>();
    RequireComponent<AnimationComponent>();
    RequireComponent<TransformComponent>();
}

void TrapAISystem::Update(const glm::vec2& playerPos, std::unique_ptr<AssetStore>& assetStore){
    for(auto& entity: GetSystemEntities()){
        const auto& position = entity.GetComponent<TransformComponent>().position;
        float distanceToPlayer = getDistanceToPlayer(position, playerPos);
        auto& aidata = entity.GetComponent<TrapAIComponent>();
        auto& pec = entity.GetComponent<ProjectileEmitterComponent>();
        auto& ac = entity.GetComponent<AnimationComponent>();
        if(distanceToPlayer <= aidata.engageRange && !aidata.igntied){ // ignite the bomb!
            aidata.igntied = true;
            ac.startTime = SDL_GetTicks();
            ac.numFrames = aidata.iginitionFrame + 1; // extra frame so it explodes when sprite dissapears
        }
        if(aidata.igntied && ac.currentFrame == aidata.iginitionFrame){
            entity.Kill();
            pec.isShooting = true;
            const auto& hpmpdata = entity.GetComponent<HPMPComponent>();
            if(hpmpdata.activehp >= -1){ //if it wasn't killed play explosion sound!
                assetStore->PlaySound(hpmpdata.deathsound);
            }
        }
    }
}

AnimatedChaseAISystem::AnimatedChaseAISystem(){
    RequireComponent<AnimatedChaseAIComponent>();
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<AnimatedShootingComponent>();
    RequireComponent<AnimationComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<SpriteComponent>();
    RequireComponent<StatusEffectComponent>();
}

void AnimatedChaseAISystem::Update(const glm::vec2& playerPos){
    for(auto& entity: GetSystemEntities()){
        const auto& position = entity.GetComponent<TransformComponent>().position;
        float distanceToPlayer = getDistanceToPlayer(position, playerPos);
        if(distanceToPlayer > 1000){continue;} // hopefully already had its stuff turned off! 
        auto& aidata = entity.GetComponent<AnimatedChaseAIComponent>();
        auto& pec = entity.GetComponent<ProjectileEmitterComponent>();
        auto& asc = entity.GetComponent<AnimatedShootingComponent>();
        auto& ac = entity.GetComponent<AnimationComponent>();
        auto& velocity = entity.GetComponent<RidigBodyComponent>().velocity;
        auto& flip = entity.GetComponent<SpriteComponent>().flip;
        if(distanceToPlayer <= aidata.detectRange){
            if(distanceToPlayer <= aidata.engageRange){
                if(distanceToPlayer <= aidata.maxDistance){ // shoot, dont chase
                    velocity.x = 0;
                    velocity.y = 0;
                    if(pec.isShooting == false){ // this is a case where enemy spawns right infront of player which is perhaps never going to occur
                        asc.animatedShooting = true;
                        pec.isShooting = true;
                        ac.xmin = 4;
                        ac.numFrames = 2; 
                        ac.currentFrame = 1;
                        ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                        pec.lastEmissionTime += pec.repeatFrequency / 2;
                    } 
                } else { // shoot, chase
                    if(chasePlayer(position, playerPos, velocity) < 0){ // facing left
                        flip = SDL_FLIP_HORIZONTAL;
                    } else { // facing right
                        flip = SDL_FLIP_NONE;
                    }  
                    if(pec.isShooting == false){
                        asc.animatedShooting = true;
                        pec.isShooting = true;
                        ac.xmin = 4;
                        ac.numFrames = 2; 
                        ac.currentFrame = 1;
                        ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                        pec.lastEmissionTime += pec.repeatFrequency / 2;
                    } 
                }
            } else { // chase, dont shoot
                if(chasePlayer(position, playerPos, velocity) < 0){ // facing left
                    flip = SDL_FLIP_HORIZONTAL;
                } else { // facing right
                    flip = SDL_FLIP_NONE;
                }
                asc.animatedShooting = false;
                pec.isShooting = false;
                ac.xmin = 0;
                if(!entity.GetComponent<StatusEffectComponent>().effects[PARALYZE]){
                    ac.numFrames = 2;   
                } else {
                    ac.numFrames = 1;
                }
            }
        } else { // dont chase or shoot
            asc.animatedShooting = false;
            pec.isShooting = false;
            ac.xmin = 0;
            ac.numFrames = 1;
            velocity.x = 0;
            velocity.y = 0;                            
        }
    }
}

AnimatedNeutralAISystem::AnimatedNeutralAISystem(){
    RequireComponent<AnimatedNeutralAIComponent>();
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<AnimatedShootingComponent>();
    RequireComponent<AnimationComponent>();
}

void AnimatedNeutralAISystem::Update(const glm::vec2& playerPos){
    for(auto& entity: GetSystemEntities()){
        const auto& position = entity.GetComponent<TransformComponent>().position;
        float distanceToPlayer = getDistanceToPlayer(position, playerPos);
        if(distanceToPlayer > 1000){continue;} // hopefully already had its stuff turned off! 
        auto& aidata = entity.GetComponent<AnimatedNeutralAIComponent>();
        auto& pec = entity.GetComponent<ProjectileEmitterComponent>();
        auto& asc = entity.GetComponent<AnimatedShootingComponent>();
        auto& ac = entity.GetComponent<AnimationComponent>();
        auto& flip = entity.GetComponent<SpriteComponent>().flip;
        if(distanceToPlayer <= aidata.engageRange){ // shoot, stand 
            pec.isShooting = true;
            asc.animatedShooting = true;
            ac.xmin = 4;
            ac.numFrames = 2;
            ac.startTime = pec.lastEmissionTime;
            playerPos.x < position.x ? flip = SDL_FLIP_HORIZONTAL : flip = SDL_FLIP_NONE;
        } else { // dont shoot; stand 
            pec.isShooting = false;
            asc.animatedShooting = false;
            ac.xmin = 0;
            ac.numFrames = 1;
            playerPos.x < position.x ? flip = SDL_FLIP_HORIZONTAL : flip = SDL_FLIP_NONE;
        }
    }
}

BossAISystem::BossAISystem(){
    RequireComponent<AnimatedShootingComponent>();
    RequireComponent<BossAIComponent>();
    RequireComponent<AnimationComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<SpriteComponent>();
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<HPMPComponent>();
    RequireComponent<RidigBodyComponent>();
}

void BossAISystem::Update(const glm::vec2& playerPos, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory, roomShut& roomToShut, const SDL_Rect& camera, const room& bossRoom){
    for(auto& entity: GetSystemEntities()){
        auto& position = entity.GetComponent<TransformComponent>().position;
        auto& velocity = entity.GetComponent<RidigBodyComponent>().velocity;
        auto& aidata = entity.GetComponent<BossAIComponent>();
        auto& asc = entity.GetComponent<AnimatedShootingComponent>();
        auto& ac = entity.GetComponent<AnimationComponent>();
        auto& flip = entity.GetComponent<SpriteComponent>().flip;
        auto& pec = entity.GetComponent<ProjectileEmitterComponent>();
        auto& hp = entity.GetComponent<HPMPComponent>().activehp;
        auto& speed = entity.GetComponent<SpeedStatComponent>().activespeed;
        auto& sec = entity.GetComponent<StatusEffectComponent>();
        auto& sprite = entity.GetComponent<SpriteComponent>();
        auto& hitnoise = entity.GetComponent<HPMPComponent>().hitsound;
        float distanceToPlayer = getDistanceToPlayer(position, playerPos);
        switch(aidata.bossType){
            case BOSSCHICKEN: { 
                if(!aidata.activated){
                    if(distanceToPlayer <= aidata.detectRange){
                        aidata.activated = true; 
                    }
                    return; // boss will activate next frame
                } 
                if(pec.isShooting == false){
                    asc.animatedShooting = true;
                    pec.isShooting = true;
                    ac.xmin = 4;
                    ac.numFrames = 2; 
                    ac.currentFrame = 1;
                    ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                    pec.lastEmissionTime += pec.repeatFrequency / 2;
                }
                if(hp > aidata.secondPhase){ // first phase
                    glm::vec2 currentDestPos = aidata.phaseOnePositions[aidata.phaseOneIndex];
                    if(std::abs(position.x - currentDestPos.x) <= 3 && std::abs(position.y - currentDestPos.y) <= 3){ // standing at destination position; change destination
                        aidata.phaseOneIndex ++;
                        if(aidata.phaseOneIndex > aidata.phaseOnePositions.size() - 1){
                            aidata.phaseOneIndex = 0;
                        }    
                    } 
                    chasePlayer(position, aidata.phaseOnePositions[aidata.phaseOneIndex], velocity);

                } else if(hp < aidata.survival){ // survival phase
                    if(!aidata.flags[0]){
                        pec.shots = 8;
                        pec.arcgap = 48;
                        aidata.flags[0] = true;
                        assetStore->PlaySound(EGGSDEATH);
                        for(int i = 0; i <= 4; i++){
                            factory->spawnMonster(registry, aidata.phaseTwoPositions[i], WHITECHICKEN);
                        }
                    }
                    if(std::abs(position.x - playerPos.x) <= 3 && std::abs(position.y - playerPos.y) <= 3){
                        velocity.x = velocity.y = 0.0;
                    } else {
                        chasePlayer(position, playerPos, velocity);    
                    }

                } else { // second phase 
                    speed = 50;
                    pec.shots = 5;
                    Uint32 time = SDL_GetTicks();
                    glm::vec2 currentDestPos = aidata.phaseTwoPositions[aidata.phaseTwoIndex];
                    if(aidata.timer0 + 1500 < time || aidata.timer0 == 0){
                        int oldIndex = aidata.phaseTwoIndex;
                        while(aidata.phaseTwoIndex == oldIndex){
                            aidata.phaseTwoIndex = RNG.randomFromRange(0,4);    
                        }
                        aidata.timer0 = time;
                    } 
                    if(std::abs(position.x - currentDestPos.x) <= 3 && std::abs(position.y - currentDestPos.y) <= 3){ // standing at destination position; dont move
                        velocity.x = velocity.y = 0.0;
                    } else{
                        chasePlayer(position, aidata.phaseTwoPositions[aidata.phaseTwoIndex], velocity);    
                    }
                    // chasePlayer(position, aidata.phaseTwoPositions[aidata.phaseTwoIndex], velocity);
                }
            } break;
            case ARCMAGE:{
                if(!aidata.activated){ 
                    if(distanceToPlayer <= 300){ // activate arcmage
                        aidata.timer0 = SDL_GetTicks();
                        assetStore->PlaySound(MNOVA);
                        aidata.activated = aidata.flags[0] = true;
                        sec.effects[INVULNERABLE] = false;
                        sprite.srcRect.y = 16*111; // moving down one to cyan sprite
                        hitnoise = GHOSTGODHIT;
                        Entity block = registry->CreateEntity();
                        switch(roomToShut.directionOfHallway){ 
                            case N:
                            case S:{ 
                                block.AddComponent<SpriteComponent>(HORIZONTALROOMBLOCKCEILINGS, 128, 64, 9, 0, 0, 0);
                                block.AddComponent<TransformComponent>(glm::vec2(roomToShut.coordiantes.x * 64, roomToShut.coordiantes.y * 64), glm::vec2(8.0, 8.0));
                                Entity walls = registry->CreateEntity();
                                walls.AddComponent<SpriteComponent>(HORIZONTALROOMBLOCKWALLS, 128, 64, 3, 0, 0, 0);
                                walls.AddComponent<TransformComponent>(glm::vec2((roomToShut.coordiantes.x) * 64, (roomToShut.coordiantes.y+1) * 64), glm::vec2(8.0, 8.0));
                                walls.AddComponent<BoxColliderComponent>(128,64);  
                                walls.Group(WALLBOX);
                            } break;
                            case W:{ 
                                block.AddComponent<SpriteComponent>(VERTICALROOMBLOCKCEILINGS, 64, 64*3, 9, 0, 0, 0);
                                block.AddComponent<BoxColliderComponent>(64, 64*4);
                                block.AddComponent<TransformComponent>(glm::vec2(roomToShut.coordiantes.x * 64, roomToShut.coordiantes.y * 64), glm::vec2(8.0, 8.0));
                                block.Group(WALLBOX);
                            } break;
                            case E:{
                                block.AddComponent<SpriteComponent>(VERTICALROOMBLOCKCEILINGS, 64, 64*3, 9, 0, 0, 0);
                                block.AddComponent<BoxColliderComponent>(64, 64*4);
                                block.AddComponent<TransformComponent>(glm::vec2((roomToShut.coordiantes.x-1) * 64, (roomToShut.coordiantes.y-1) * 64), glm::vec2(8.0, 8.0));
                                block.Group(WALLBOX);
                            } break;
                        }
                        asc.animatedShooting = true;
                        pec.isShooting = true;
                        ac.xmin = 4;
                        ac.numFrames = 2; 
                        ac.currentFrame = 1;
                        ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                        pec.lastEmissionTime += pec.repeatFrequency / 2;
                    }
                } else { // boss is activated. phases
                    Uint32 time = SDL_GetTicks();
                    if(hp > aidata.secondPhase){ // first phase
                        if(time >= aidata.timer0 + 10000 && time - pec.lastEmissionTime > pec.repeatFrequency){ // shoot circle of stars every 10 seconds
                            aidata.timer0 = time;
                            arcMageConfuseShots(entity, position, registry, aidata.phaseOnePositions);
                        }

                    } else if (hp < aidata.survival) { // survival phase

                        if(!aidata.flags[0]){ // flag 0 indicates if arcMage has reached center of room
                            if(!(std::abs(position.x - aidata.phaseTwoPositions[1].x) <= 3 && std::abs(position.y - aidata.phaseTwoPositions[1].y) <= 3)){
                                chasePlayer(position, aidata.phaseTwoPositions[1], velocity);
                                if(!aidata.flags[3]){
                                    assetStore->PlaySound(MNOVA);
                                    sec.effects[INVULNERABLE] = true;
                                    sprite.srcRect.y = 16*110;
                                    hitnoise = VOIDHIT;
                                    aidata.timer1 = pec.shots = asc.animatedShooting = pec.isShooting = 0;
                                    ac.xmin = 1;
                                    !entity.GetComponent<StatusEffectComponent>().effects[PARALYZE] ? ac.numFrames = 2 : ac.numFrames = 1;
                                    position.x > aidata.phaseTwoPositions[1].x ? flip = SDL_FLIP_HORIZONTAL : flip = SDL_FLIP_NONE;
                                    ac.frameSpeedRate = 4;
                                    aidata.flags[3] = true;
                                }
                            } else {
                                aidata.flags[0] = aidata.flags[1] = aidata.flags[2] = true;
                                velocity = {0,0};    
                            }
                            continue; // dont really start survival phase until arrived at center of room
                        }

                        // standing at middle of room. survival phase 
                        if(aidata.flags[1]){ // flag 1 used to indicate first frame of survival phase at center of room
                            if(aidata.flags[3]){ // arcMage did run without shooting animation, re-enable shooting
                                ac.frameSpeedRate = 2000 / pec.repeatFrequency; 
                                asc.animatedShooting = true;
                                pec.isShooting = true;
                                ac.xmin = 4;
                                ac.numFrames = 2; 
                                ac.currentFrame = 1;
                                pec.lastEmissionTime = 0;
                                ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                                pec.lastEmissionTime += pec.repeatFrequency / 2; 
                            }
                            aidata.flags[1] = false;         
                            pec.shots = 24;
                            pec.arcgap = 360;
                            aidata.timer0 = 0;
                        }

                        // every 3 seconds spawn, swap vulnerability, emit confuse stars
                        if(time >= aidata.timer0 + 3000 && time - pec.lastEmissionTime > pec.repeatFrequency){
                            aidata.timer0 = time;
                            assetStore->PlaySound(MNOVA);
                            arcMageConfuseShots(entity, position, registry, aidata.phaseOnePositions);
                            arcMageSpawnMinionsSurvival(registry, factory, bossRoom);
                            if(sec.effects[INVULNERABLE]){
                                sec.effects[INVULNERABLE] = false;
                                sprite.srcRect.y = 16*111;
                                hitnoise = GHOSTGODHIT;
                            } else {
                                sec.effects[INVULNERABLE] = true;
                                sprite.srcRect.y = 16*110;
                                hitnoise = VOIDHIT;
                            }
                        }


                    } else { // second phase
                        if(aidata.flags[0]){ // flag0 used in phase 2 to indicate first frame of phase 2 
                            aidata.flags[0] = false;
                            RNG.randomFromRange(0,1) == 0 ? aidata.phaseTwoIndex = 0 : aidata.phaseTwoIndex = 2;
                            chasePlayer(position, aidata.phaseTwoPositions[aidata.phaseTwoIndex], velocity);
                            aidata.positionflag = aidata.phaseTwoPositions[aidata.phaseTwoIndex];
                            // positionflag used to keep track of where arcMage is attempting to go
                        }

                        int timeAtSpot = 10000;
                        velocity = {0,0}; // modified in cases where chasePlayer called
                        if((aidata.positionflag == aidata.phaseTwoPositions[0] && (std::abs(position.x - aidata.phaseTwoPositions[0].x) <= 3) && std::abs(position.y - aidata.phaseTwoPositions[0].y) <= 3) 
                        || (aidata.positionflag == aidata.phaseTwoPositions[2] && (std::abs(position.x - aidata.phaseTwoPositions[2].x) <= 3) && std::abs(position.y - aidata.phaseTwoPositions[2].y) <= 3)){ 
                        // successfully arrived at either edge 
                            if(!aidata.flags[1]){ // first frame of edge arrival 
                                aidata.flags[1] = true; // flag1 used in second phase to indicate successfully arriving at destination    
                                aidata.timer0 = time; // time0 used in second phase to track time since successfully arriving at destination
                                sec.effects[INVULNERABLE] = true;
                                sprite.srcRect.y = 16*110;
                                hitnoise = VOIDHIT;
                                pec.shots = 12;
                                pec.arcgap = 180;

                                // emit wall shots, spawn some dudes 
                                arcMageWallShots(entity, registry, bossRoom);
                                arcMageSpawnMinionsPhaseTwo(registry, factory, bossRoom);
                                assetStore->PlaySound(MNOVA);
                                
                            }
                        } else if(aidata.positionflag == aidata.phaseTwoPositions[1] && (std::abs(position.x - aidata.phaseTwoPositions[1].x) <= 3) && std::abs(position.y - aidata.phaseTwoPositions[1].y) <= 3){
                        //successfully arrived at center
                            timeAtSpot = 5000;
                            if(!aidata.flags[1]){
                                aidata.flags[1] = true; // flag1 used in second phase to indicate successfully arriving at destination    
                                aidata.timer0 = time; // time0 used to track time since successfully arriving at destination
                            }
                            if(!aidata.flags[2] && time - pec.lastEmissionTime > pec.repeatFrequency){
                                arcMageConfuseShots(entity, position, registry, aidata.phaseOnePositions);
                                aidata.flags[2] = true; // flags2 used in second phase to shoot confuse stars when at center of room
                            }
                        } else if(!aidata.flags[1]){ // flag1 used in second phase to indicate successfully arriving at destination   
                        // not arrived anywhere; keep moving to current destination
                            chasePlayer(position, aidata.phaseTwoPositions[aidata.phaseTwoIndex], velocity);
                        }

                        // destination successfully reached and time spent there elapsed; time to move to new destination 
                        if(aidata.flags[1]&& time >= aidata.timer0 + timeAtSpot){ 
                            aidata.flags[2] = aidata.flags[1] = false;
                            aidata.phaseTwoIndex++;
                            if(aidata.phaseTwoIndex > 3){aidata.phaseTwoIndex = 0;}
                            chasePlayer(position, aidata.phaseTwoPositions[aidata.phaseTwoIndex], velocity);
                            aidata.positionflag = aidata.phaseTwoPositions[aidata.phaseTwoIndex];
                            if(aidata.phaseTwoIndex == 1 || aidata.phaseTwoIndex == 3){ // leaving wall
                                assetStore->PlaySound(MNOVA);
                            }
                            sprite.srcRect.y = 16*111; 
                            hitnoise = GHOSTGODHIT;
                            sec.effects[INVULNERABLE] = false;
                            pec.shots = 5;
                            pec.arcgap = 95;
                        }
                        
                    }

                }

            } break;
            case GORDON:{
                if(!aidata.activated){ // boss stands invulnerable for 2.5s
                    if(!aidata.flags[0]){
                        aidata.timer0 = SDL_GetTicks() + 2500;
                        aidata.flags[0] = true;
                    }
                    if(SDL_GetTicks() >= aidata.timer0){
                        sec.effects[INVULNERABLE] = aidata.flags[0] = false;
                        aidata.activated = true;
                        pec.isShooting = asc.animatedShooting = true;
                        ac.xmin = 4;
                        ac.numFrames = 2;
                        ac.currentFrame = 1;
                        ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                        pec.lastEmissionTime += pec.repeatFrequency / 2; 
                        aidata.timer0 = SDL_GetTicks();
                        aidata.flags[0] = false;
                        starShotgun(entity, registry, playerPos);
                        // hp -= 20000;
                    }
                } else { // boss activated: phases
                    auto time = SDL_GetTicks();
                    if(hp > aidata.secondPhase){ // phase one 
                        if(time > aidata.timer0 + 5000 && time - pec.lastEmissionTime > pec.repeatFrequency){
                            aidata.timer0 = time;
                            starShotgun(entity, registry, playerPos);
                        }

                    } else if(hp < aidata.survival){ // survival
                        if(aidata.flags[0]){ // first frame of survival
                            aidata.flags[0] = false;
                            aidata.positionflag = playerPos;
                            ac.xmin = 0;
                            ac.numFrames = 3;
                            ac.currentFrame = 1;
                            ac.frameSpeedRate = 2000 / 500;
                            aidata.flags[1] = true; // flag[1] = true means chasing, else shooting at center
                            
                        }

                        if(aidata.flags[1]){ // flag[1] = chasing sequence
                            chasePlayer(position, aidata.positionflag, velocity);
                            if(!aidata.flags[2] && ((std::abs(position.x - aidata.positionflag.x) <= 3 && std::abs(position.y - aidata.positionflag.y) <= 3) || entity.GetComponent<CollisionFlagComponent>().collisionFlag != NONESIDE)){
                                aidata.flags[2] = true; // flags[2] means player reached during first part of chase
                                gordonSurvivalShotGun(entity, registry, playerPos);
                            }
                            if(aidata.flags[2]){
                                aidata.positionflag = aidata.spawnPoint;
                                if(std::abs(position.x - aidata.positionflag.x) <= 3 && std::abs(position.y - aidata.positionflag.y) <= 3){ // arrived at center again
                                    Entity gs = factory->spawnMonster(registry, aidata.spawnPoint, GIGASHEEP);
                                    registry->GetComponent<BoxColliderComponent>(gs.GetId()).offset.y -= 16;
                                    registry->GetComponent<TransformComponent>(gs.GetId()).scale = {10,10};
                                    aidata.timer0 = time;
                                    aidata.flags[2] = aidata.flags[1] = false;
                                }
                            }
                            // if arrived at player, set new position target to spawnPoint
                        } else { // go to center and shoot for X seconds    
                            if(!aidata.flags[2]){ // first frame of standign at center
                                aidata.flags[2] = pec.isShooting = asc.animatedShooting = true;
                                ac.xmin = 4;
                                ac.numFrames = 2;
                                ac.currentFrame = 1;
                                // ac.frameSpeedRate = 2000 / pec.repeatFrequency;
                                ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                                pec.lastEmissionTime += pec.repeatFrequency / 2;
                                aidata.timer0 = time;
                            }
                            if(time > aidata.timer0 + 10000){
                                ac.xmin = 0;
                                ac.numFrames = 3;
                                ac.currentFrame = 1;
                                aidata.flags[1] = true; // start chasing sequence
                                pec.isShooting = aidata.flags[2] = false;
                                aidata.positionflag = playerPos;
                            }
                            velocity = {0.0,0.0};
                        }

                    } else { // phase two
                        if(!aidata.flags[0]){ // first frame of phase two: update pec.rf
                            pec.repeatFrequency = 500;
                            aidata.timer0 = time - pec.repeatFrequency;
                            ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                            pec.isShooting = false;
                            aidata.flags[0] = true;
                            sec.effects[INVULNERABLE] = true;
                            Entity key = factory->spawnMonster(registry, position, SHEEP);
                            aidata.idKeyOne = key.GetId();
                            aidata.cIdKeyOne = key.GetCreationId();
                            key = factory->spawnMonster(registry, position, SHEEP);
                            aidata.idKeyTwo = key.GetId();
                            aidata.cIdKeyTwo = key.GetCreationId();
                            aidata.phaseTwoIndex = 35;
                            registry->Update();
                            // hp -= 29000;
                        }
                        bool key1alive = registry->GetCreationIdFromEntityId(aidata.idKeyOne) == aidata.cIdKeyOne;
                        bool key2alive = registry->GetCreationIdFromEntityId(aidata.idKeyTwo) == aidata.cIdKeyTwo;
                        if(key1alive || key2alive){
                            if(time > aidata.timer0 + 500){
                                aidata.timer0 = time;
                                gordonPhaseTwoShots(entity, registry, aidata.phaseThreeIndex, aidata.phaseOnePositions);
                                // phase three index used to select origin. its overflow is controlled by reference in gordonPhaseTwoShots
                            }
                            if(key1alive){
                                const auto& key1pos = registry->GetComponent<TransformComponent>(aidata.idKeyOne).position;
                                auto& key1velocity = registry->GetComponent<RidigBodyComponent>(aidata.idKeyOne).velocity;
                                glm::vec2 key1destination = aidata.phaseOnePositions[aidata.phaseOneIndex];
                                if(std::abs(key1pos.x - key1destination.x) <= 3 && std::abs(key1pos.y - key1destination.y) <= 3){ // standing at destination position; change destination
                                    aidata.phaseOneIndex ++;
                                    if(aidata.phaseOneIndex > aidata.phaseOnePositions.size() - 1){
                                        aidata.phaseOneIndex = 0;
                                    }    
                                }
                                chasePlayer(key1pos, aidata.phaseOnePositions[aidata.phaseOneIndex], key1velocity); 
                                auto& key1flip = registry->GetComponent<SpriteComponent>(aidata.idKeyOne).flip;
                                playerPos.x <= key1pos.x ? key1flip = SDL_FLIP_HORIZONTAL : key1flip = SDL_FLIP_NONE;
                                aidata.timer1 = time;
                            }
                            if(key2alive){
                                const auto& key2pos = registry->GetComponent<TransformComponent>(aidata.idKeyTwo).position;
                                auto& key2velocity = registry->GetComponent<RidigBodyComponent>(aidata.idKeyTwo).velocity;
                                glm::vec2 key2destination = aidata.phaseTwoPositions[aidata.phaseTwoIndex];
                                if(std::abs(key2pos.x - key2destination.x) <= 3 && std::abs(key2pos.y - key2destination.y) <= 3){ // standing at destination position; change destination
                                    aidata.phaseTwoIndex --;
                                    if(aidata.phaseTwoIndex < 0){
                                        aidata.phaseTwoIndex = 35;
                                    }    
                                }
                                chasePlayer(key2pos, aidata.phaseTwoPositions[aidata.phaseTwoIndex], key2velocity); 
                                auto& key2flip = registry->GetComponent<SpriteComponent>(aidata.idKeyTwo).flip; 
                                playerPos.x <= key2pos.x ? key2flip = SDL_FLIP_HORIZONTAL : key2flip = SDL_FLIP_NONE;
                                aidata.timer1 = time;
                            }
                        } else { // keys are dead
                            pec.isShooting = false;
                            ac.currentFrame = 1;
                            ac.numFrames = 1;
                            ac.xmin = 2;
                            sec.effects[INVULNERABLE] = false;
                            if(time > aidata.timer1 + 7500){ // damage phase ended. re-spawn keys and shoot again 
                                pec.repeatFrequency = 500; 
                                aidata.timer0 = time - pec.repeatFrequency;
                                ac.xmin = 4;
                                ac.numFrames = 2;
                                ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                                pec.lastEmissionTime += pec.repeatFrequency / 2;
                                sec.effects[INVULNERABLE] = true;
                                Entity key = factory->spawnMonster(registry, position, SHEEP);
                                aidata.idKeyOne = key.GetId();
                                aidata.cIdKeyOne = key.GetCreationId();
                                key = factory->spawnMonster(registry, position, SHEEP);
                                aidata.idKeyTwo = key.GetId();
                                aidata.cIdKeyTwo = key.GetCreationId();
                                aidata.phaseTwoIndex = 35;
                                registry->Update();
                            }
                        }
                    }
                }
            } break;
        }
        playerPos.x <= position.x ? flip = SDL_FLIP_HORIZONTAL : flip = SDL_FLIP_NONE;
    }
}

AnimatedPounceAISystem::AnimatedPounceAISystem(){
    RequireComponent<AnimatedPounceAIComponent>();
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<AnimatedShootingComponent>();
    RequireComponent<AnimationComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<SpriteComponent>();
    RequireComponent<StatusEffectComponent>();
    RequireComponent<SpeedStatComponent>();
}

void AnimatedPounceAISystem::Update(const glm::vec2& playerPos){
    for(auto& entity: GetSystemEntities()){
        const auto& position = entity.GetComponent<TransformComponent>().position;
        float distanceToPlayer = getDistanceToPlayer(position, playerPos);
        if(distanceToPlayer > 1000){continue;} // hopefully already had its stuff turned off! 
        auto& aidata = entity.GetComponent<AnimatedPounceAIComponent>();
        auto& pec = entity.GetComponent<ProjectileEmitterComponent>();
        auto& asc = entity.GetComponent<AnimatedShootingComponent>();
        auto& ac = entity.GetComponent<AnimationComponent>();
        auto& velocity = entity.GetComponent<RidigBodyComponent>().velocity;
        auto& flip = entity.GetComponent<SpriteComponent>().flip;
        auto& speed = entity.GetComponent<SpeedStatComponent>().activespeed;
        if(distanceToPlayer <= aidata.detectRange){ 
            if(distanceToPlayer <= aidata.engageRange){ // pounce if possible!
                auto time = SDL_GetTicks();
                if(!aidata.pouncing){
                    if(time > aidata.lastPounceTime + aidata.pounceCooldown){ // not pouncing & pouncing cooldown expired
                        aidata.pouncing = true;
                        aidata.lastPounceTime = time;
                    } 
                    if(distanceToPlayer < 50){ 
                        velocity.x = velocity.y = 0;
                        if(pec.isShooting){
                            if(pec.lastEmissionTime >= aidata.lastPounceTime + pec.repeatFrequency - pec.repeatFrequency / 2){ // shot completed
                                asc.animatedShooting = pec.isShooting = false;
                            }
                            // ac.xmin = 4;
                            // ac.numFrames = 2; 
                            // ac.currentFrame = 1;
                        } else {
                            // velocity.x = velocity.y = 0;
                            ac.xmin = 0;
                            ac.numFrames = 1;
                        }
                    } else {
                        speed = aidata.speeds[0];
                        if(pec.isShooting){
                            if(pec.lastEmissionTime >= aidata.lastPounceTime + pec.repeatFrequency - pec.repeatFrequency / 2){ // shot completed
                                asc.animatedShooting = pec.isShooting = false;
                            }
                            // ac.xmin = 4;
                            // ac.numFrames = 2; 
                            // ac.currentFrame = 1;
                        } else {
                            ac.xmin = 0;
                            entity.GetComponent<StatusEffectComponent>().effects[PARALYZE] ? ac.numFrames = 1 : ac.numFrames = 2;
                        }
                        chasePlayer(position, playerPos, velocity); 

                    }
                } else {
                    if(time >= aidata.lastPounceTime + 1000){ // end pounce: time elapsed  (walk)
                        speed = aidata.speeds[0];
                        asc.animatedShooting = pec.isShooting = true;
                        // ac.xmin = 4;
                        // ac.numFrames = 2; 
                        // ac.currentFrame = 1;
                        ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                        pec.lastEmissionTime += pec.repeatFrequency / 2;
                        chasePlayer(position, playerPos, velocity);
                        aidata.pouncing = false;
                        aidata.lastPounceTime = time;
                    } else if (distanceToPlayer < 50){ // end pounce: target reached (stand)
                        asc.animatedShooting = pec.isShooting = true;
                        // ac.xmin = 4;
                        // ac.numFrames = 2; 
                        // ac.currentFrame = 1;
                        ac.startTime = pec.lastEmissionTime = SDL_GetTicks() - pec.repeatFrequency;
                        pec.lastEmissionTime += pec.repeatFrequency / 2;
                        velocity.x = velocity.y = 0;
                        aidata.pouncing = false; 
                        aidata.lastPounceTime = time;
                    } else { //continue pounce
                        asc.animatedShooting = false;
                        pec.isShooting = false;
                        ac.xmin = 0;
                        entity.GetComponent<StatusEffectComponent>().effects[PARALYZE] ? ac.numFrames = 1 : ac.numFrames = 2;
                        // aidata.lastPounceTime = time;
                        speed = aidata.speeds[2];
                        chasePlayer(position, playerPos, velocity);    
                    }
                }
                playerPos.x <= position.x ? flip = SDL_FLIP_HORIZONTAL : flip = SDL_FLIP_NONE;
            } else { // chase at regular speed. dont shoot or pounce
                speed = aidata.speeds[1];
                if(chasePlayer(position, playerPos, velocity) < 0){ // facing left
                    flip = SDL_FLIP_HORIZONTAL;
                } else { // facing right
                    flip = SDL_FLIP_NONE;
                }
                asc.animatedShooting = false;
                pec.isShooting = false;
                ac.xmin = 0;
                if(!entity.GetComponent<StatusEffectComponent>().effects[PARALYZE]){
                    ac.numFrames = 2;   
                } else {
                    ac.numFrames = 1;
                }
            }
        } else { // out of range; do nothing
            asc.animatedShooting = false;
            pec.isShooting = false;
            ac.xmin = 0;
            ac.numFrames = 1;
            velocity.x = 0;
            velocity.y = 0;                            
        }
    }
}