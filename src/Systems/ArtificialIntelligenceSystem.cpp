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

void BossAISystem::Update(const glm::vec2& playerPos, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory){
    for(auto& entity: GetSystemEntities()){
        const auto& position = entity.GetComponent<TransformComponent>().position;
        auto& velocity = entity.GetComponent<RidigBodyComponent>().velocity;
        auto& aidata = entity.GetComponent<BossAIComponent>();
        auto& asc = entity.GetComponent<AnimatedShootingComponent>();
        auto& ac = entity.GetComponent<AnimationComponent>();
        auto& flip = entity.GetComponent<SpriteComponent>().flip;
        auto& pec = entity.GetComponent<ProjectileEmitterComponent>();
        const auto& hp = entity.GetComponent<HPMPComponent>().activehp;
        auto& speed = entity.GetComponent<SpeedStatComponent>().activespeed;
        if(!aidata.activated){
            float distanceToPlayer = getDistanceToPlayer(position, playerPos);
            if(distanceToPlayer <= aidata.detectRange){
                aidata.activated = true; 
            }
            return; // boss will activate next frame
        } 
        switch(aidata.bossType){
            case BOSSCHICKEN: { 
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
                    if(!aidata.flag0){
                        pec.shots = 8;
                        pec.arcgap = 48;
                        aidata.flag0 = true;
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
            case ARCMAGEBOSSAI:{

            } break;
            case GORDONBOSSAI:{

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