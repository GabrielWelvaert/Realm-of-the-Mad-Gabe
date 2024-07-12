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
            auto& SPEC = entity.GetComponent<SecondaryProjectileComponent>();
            if(time - SPEC.lastEmissionTime > SPEC.repeatFrequency){
                const auto& PEC = entity.GetComponent<ProjectileEmitterComponent>();
                if(time - PEC.lastEmissionTime > PEC.repeatFrequency){
                    const auto& spriteEnum = PEC.emitterSprite;
                    const auto& monsterPos = entity.GetComponent<TransformComponent>().position;
                    SPEC.lastEmissionTime = time; 
                    switch(spriteEnum){
                        case HENCHDEMON0:{
                            henchmanblueshotgun(entity, playerPos, registry);
                        } break;
                        case SLIMEGOD:{
                            slimeGodSlow(entity, playerPos, registry);
                        } break;
                        case BEHOLDER:{
                            beholderBlind(entity, playerPos, registry);
                        } break;
                        case SPRITEGOD:{
                            spriteGodBoomerang(entity, playerPos, registry);
                        } break;
                        case MEDUSA:{
                            MedusaBomb(entity, playerPos, registry, spriteEnum);
                        } break;
                        case CUBEGOD:{
                            cubeGodShotgun(entity, playerPos, registry);
                        } break;
                        case CYANCUBE:{
                            slowedStar(entity, playerPos, registry);
                        } break;
                        case ORANGECUBE:{
                            fireBolt(entity, playerPos, registry);
                        } break;
                        case SKELETON5:{
                            if(RNG.randomBool()){
                                confusedStarShotgeun(entity, playerPos, registry);
                            } else {
                                stunedStarShotgun(entity, playerPos, registry);
                            }
                        } break;
                        case PENTARACTTOWER:{
                            MedusaBomb(entity, playerPos, registry, spriteEnum, 130);
                        } break;
                        case GHOSTASSASSIN:{
                            MedusaBomb(entity, playerPos, registry, spriteEnum, 25);
                        } break;
                        case GHOSTARCHER:{
                            bigArrow(entity, playerPos, registry);
                        } break;
                        case CRUSADER3:
                        case BLUESPIRIT:{
                            MedusaBomb(entity, playerPos, registry, spriteEnum, 35);
                        } break;
                        case SHADE:{
                            paralyzedStar(entity, playerPos, registry);
                        } break;
                        case BOSSCHICKEN:{
                            chicken(entity, playerPos, registry);
                        } break;
                        case ABYSSTOWER:{
                            blackTowerBomb(entity, playerPos, registry, spriteEnum);
                        } break;
                        case LEVIATHAN:{
                            LeviathanShotGun(entity, playerPos, registry);
                        } break;
                        case HELLGOLEM:{
                            blackTowerBomb(entity, playerPos, registry, spriteEnum);
                            hellgolemshotgun(entity, playerPos, registry);
                        } break;
                    }
                }
            }
        }
    }
}
