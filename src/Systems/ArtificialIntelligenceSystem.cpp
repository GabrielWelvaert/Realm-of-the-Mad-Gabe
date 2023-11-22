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

void PassiveAISystem::Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore){return;}

ChaseAISystem::ChaseAISystem(){
    RequireComponent<ChaseAIComponent>();
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<TransformComponent>();
}

void ChaseAISystem::Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore){
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

void NeutralAISystem::Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore){
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

void TrapAISystem::Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore){
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

void AnimatedChaseAISystem::Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore){
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

void AnimatedNeutralAISystem::Update(glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore){
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