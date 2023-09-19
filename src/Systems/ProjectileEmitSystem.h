#ifndef PROJECTILEEMITSYSTEM_H
#define PROJECTILEEMITSYSTEM_H

#include "../Components/ProjectileEmitterComponent.h"
#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include <unordered_map>
#include "../utils/enums.h"
#include "../utils/Xoshiro256.h"

/*
This system is responsible for emitting projectiles when enough time has passed for successive shot(s) to be fired
It also contains the algorithm for shots with multiple projectiles
*/

class ProjectileEmitSystem: public System{
    private:
        Xoshiro256 RNG;

    public:
        ProjectileEmitSystem(){
            RequireComponent<ProjectileEmitterComponent>();
            RequireComponent<TransformComponent>();
            // RequireComponent<OffenseStatComponent>();
            RequireComponent<SpriteComponent>();
        }

        // given projectile origin and destination coordiantes and ProjectileEmitterComponent.velocity reference
        // 1) updates ProjectileEmitterComponent velocity 
        // 2) returns degrees of rotation to be added to projectile's transformComponent.rotation 
        // diagonal parameter is a flag to adjust the angle for sprites that start with +45 degrees
        // assumes projectile sprite is: ↑ and a diagonal projectile is ↗
        inline float getRotationFromCoordiante(const float& projectileSpeed, const float& originX, const float& originY, const float& destX, const float& destY, glm::vec2& emitterVelocity, const bool& diagonal = false){
            float angleRadians = std::atan2(destY - originY, destX - originX);   
            float angleDegrees = angleRadians * (180.0 / M_PI);
            emitterVelocity.x = projectileSpeed * std::cos(angleRadians);
            emitterVelocity.y = projectileSpeed * std::sin(angleRadians);
            return fmod(angleDegrees + 90.0, 360.0) - 45*diagonal; // fmod shit because degrees=0 is top right
        }

        // derives velocity for arc-gap-difference projectiles given the origin projectile velocity
        inline void projectileVelocityArcGap(const glm::vec2& originVelocity, const float& rotationDegrees, const float& deltaDegrees, glm::vec2& emitterVelocity){
            float deltaRadians = deltaDegrees * (M_PI / 180.0);
            emitterVelocity.x = originVelocity.x * std::cos(deltaRadians) - originVelocity.y * std::sin(deltaRadians);
            emitterVelocity.y = originVelocity.x * std::sin(deltaRadians) + originVelocity.y * std::cos(deltaRadians);
        }

        void Update(std::unique_ptr<Registry>& registry, SDL_Rect camera, int mx, int my, glm::vec2 playerPos, std::unique_ptr<AssetStore>& assetStore) {
            for (auto entity: GetSystemEntities()){
                auto& PEC = entity.GetComponent<ProjectileEmitterComponent>();
                if(PEC.isShooting){
                    if (SDL_GetTicks() - PEC.lastEmissionTime > PEC.repeatFrequency){ // shoot time; emit projectile!
                        // std::cout << SDL_GetTicks() - PEC.lastEmissionTime << std::endl;
                        const int projectilescale = 5;
                        const auto& duration = PEC.duration;
                        const auto& piercing = PEC.piercing;
                        const auto& arcgap = PEC.arcgap;
                        int shots = PEC.shots;
                        const bool IsPlayer = entity.GetId() == 0;
                        const auto& parentSprite = entity.GetComponent<SpriteComponent>(); // sprite of projectile's parent
                        const auto& transform = entity.GetComponent<TransformComponent>(); // transform of projectile's parent
                        const soundEnums playerSounds[12] = {BLADESWING, ARROWSHOOT, MAGICSHOOT, MAGICSHOOT, BLADESWING, BLADESWING, BLADESWING, BLADESWING, MAGICSHOOT, ARROWSHOOT, MAGICSHOOT, BLADESWING};
                        // calculating origin for projectile(s)
                        const auto& isDiagonal = PEC.spritediagonalSprite;
                        auto playerCenterX = transform.position.x + ((parentSprite.width * transform.scale.x) / 2) - PEC.spritewidth * projectilescale / 2; 
                        auto playerCenterY = transform.position.y + ((parentSprite.height * transform.scale.y) / 2); // no sprite mod?
                        auto projectilePosition = glm::vec2(playerCenterX, playerCenterY); 
                        //@@@@
                        
                        if(isDiagonal){ // diagonal sprites are huge need to adjust spawn position slightly
                            projectilePosition.y -= 10;
                        }

                        unsigned long damage;
                        unsigned char parentGroupEnumInt;

                        // get origin data
                        float realgap;
                        float rotationDegrees = 0.0;
                        Entity projectile;
                        glm::vec2 originVelocity;
                        if(IsPlayer){                                                      // value removed from camera.y should be derived from projectile sprite dimension
                            rotationDegrees = getRotationFromCoordiante(PEC.projectileSpeed, projectilePosition.x, projectilePosition.y, mx+camera.x, my+camera.y-18, originVelocity, isDiagonal);
                            const auto& activeattack = entity.GetComponent<OffenseStatComponent>().activeattack;
                            float damageCalc = static_cast<float>(damage)*((static_cast<float>(activeattack)+25)/50); // must cast to float so math works
                            damage = damageCalc; // damage rounds down
                            parentGroupEnumInt = 4; // hardcoded. must be changed if groups enum is altered!
                            damage = RNG.randomFromRange(PEC.minDamage, PEC.damage);
                        } else{
                            rotationDegrees = getRotationFromCoordiante(PEC.projectileSpeed, projectilePosition.x, projectilePosition.y, playerPos.x, playerPos.y, originVelocity, isDiagonal); 
                            parentGroupEnumInt = 0; // hardcoded. must be changed if groups enum is altered!
                            damage = PEC.damage;
                            
                        }                                                                                                       // should target centerplayer not playerpos?
                        
                        if(shots % 2 == 1){ // odd number of shots; shoot one at origin!
                            projectile = registry->CreateEntity();
                            projectile.AddComponent<RidigBodyComponent>(originVelocity);
                            projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                            projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                            projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), rotationDegrees); //init pos is same as emitter 
                            PEC.lastEmissionTime = SDL_GetTicks(); // we just shot so record that in lastEmissionTime
                            projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt);
                            projectile.Group(PROJECTILE);
                            if(shots == 1){ // only one shot just shoot and origin and exit
                                if(IsPlayer){
                                    const auto& classname = entity.GetComponent<ClassNameComponent>().classname; 
                                    assetStore->PlaySound(playerSounds[classname]);
                                }
                                PEC.lastEmissionTime = SDL_GetTicks();
                                continue;
                            }
                            realgap = arcgap / shots;
                            shots -= 1;
                            for(int i = 1; i <= shots / 2; i++){ // shoot the rest of the shots in odd numshot case
                                // next above origin
                                projectile = registry->CreateEntity();
                                projectile.AddComponent<RidigBodyComponent>();
                                auto& velocity1 = projectile.GetComponent<RidigBodyComponent>().velocity;
                                projectileVelocityArcGap(originVelocity, rotationDegrees, realgap*i, velocity1);
                                projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                                projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), rotationDegrees + realgap*i); //init pos is same as emitter 
                                projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt);
                                projectile.Group(PROJECTILE);
                                
                                // next below origin
                                projectile = registry->CreateEntity();
                                projectile.AddComponent<RidigBodyComponent>();
                                auto& velocity2 = projectile.GetComponent<RidigBodyComponent>().velocity;
                                projectileVelocityArcGap(originVelocity, rotationDegrees, realgap*-i, velocity2);
                                projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                                projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), rotationDegrees + realgap*-i); //init pos is same as emitter 
                                projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt);
                                projectile.Group(PROJECTILE);
                            }

                        } else { // even number of shots
                            realgap = arcgap / (shots-1);
                            for(int i = 0; i < shots/2; i++){
                                bool first = i != 0;

                                projectile = registry->CreateEntity();
                                projectile.AddComponent<RidigBodyComponent>();
                                auto& velocity1 = projectile.GetComponent<RidigBodyComponent>().velocity;
                                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/2) + realgap*i*first, velocity1);
                                projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                                projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), (rotationDegrees + realgap/2) + realgap*i*first); //init pos is same as emitter 
                                projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt);
                                projectile.Group(PROJECTILE);

                                projectile = registry->CreateEntity();
                                projectile.AddComponent<RidigBodyComponent>();
                                auto& velocity2 = projectile.GetComponent<RidigBodyComponent>().velocity;
                                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/-2) + realgap*-i*first, velocity2);
                                projectile.AddComponent<SpriteComponent>(PEC.spriteassetId, PEC.spritewidth, PEC.spriteheight, PEC.spritesrcRect, PEC.spritezIndex, PEC.spriteisFixed, PEC.spritediagonalSprite);
                                projectile.AddComponent<BoxColliderComponent>(PEC.boxwidth, PEC.boxheight, PEC.boxoffset);
                                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(projectilescale,projectilescale), (rotationDegrees - realgap/2) + realgap*-i*first); //init pos is same as emitter 
                                projectile.AddComponent<ProjectileComponent>(damage, duration, piercing, entity, parentGroupEnumInt);
                                projectile.Group(PROJECTILE);
                            }

                        }
                        
                        if(IsPlayer){ // in any case...
                            const auto& classname = entity.GetComponent<ClassNameComponent>().classname;
                            assetStore->PlaySound(playerSounds[classname]);
                        }
                        PEC.lastEmissionTime = SDL_GetTicks();
                        
                    }
                }    
                
            }
        }
};


#endif