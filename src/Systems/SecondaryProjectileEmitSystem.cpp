#include "SecondaryProjectileEmitSystem.h"

SecondaryProjectileEmitSystem::SecondaryProjectileEmitSystem(){
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<SecondaryProjectileComponent>();
    RequireComponent<TransformComponent>(); 
    RequireComponent<isShootingComponent>();
}


void SecondaryProjectileEmitSystem::Update(const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
    auto time = SDL_GetTicks();
    for(auto& entity: GetSystemEntities()){
        const auto& isShooting = entity.GetComponent<isShootingComponent>().isShooting;
        if(isShooting){
            const auto& PEC = entity.GetComponent<ProjectileEmitterComponent>();
            auto& SPEC = entity.GetComponent<SecondaryProjectileComponent>();
            if(time - SPEC.lastEmissionTime > SPEC.repeatFrequency && time - PEC.lastEmissionTime > PEC.repeatFrequency){
                const auto& spriteEnum = PEC.spriteOfParent;
                const auto& monsterPos = entity.GetComponent<TransformComponent>().position;
                SPEC.lastEmissionTime = time;
                switch(spriteEnum){
                    case SLIMEGOD:{
                        slimeGodSlow(entity, playerPos, monsterPos, registry);
                    } break;
                    case BEHOLDER:{
                        beholderBlind(entity, playerPos, monsterPos, registry);
                    } break;
                    case SPRITEGOD:{
                        spriteGodBoomerang(entity, playerPos, monsterPos, registry);
                    } break;
                    
                }
            }
        }
    }
}
