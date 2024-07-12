#ifndef SECONDARYPROJECTILEEMITSYSTEM_H
#define SECONDARYPROJECTILEEMITSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/SecondaryProjectileComponent.h"
#include "../Components/LinearProjectileComponent.h"
#include "../Components/OscillatingProjectileComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/LinearProjectileComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../../libs/SDL2/SDL.h"
#include "../Components/isShootingComponent.h"
#include "../Components/RotationComponent.h"
#include "../Components/ParabolicMovementComponent.h"
#include "../Utils/Xoshiro256.h"


class SecondaryProjectileEmitSystem: public System{
    private:
        Xoshiro256 RNG;

        inline float getRotationFromCoordiante(const float& projectileSpeed, const float& originX, const float& originY, const float& destX, const float& destY, glm::vec2& emitterVelocity, const bool& diagonal = false){
            float angleRadians = std::atan2(destY - originY, destX - originX);   
            float angleDegrees = angleRadians * (180.0 / M_PI);
            emitterVelocity.x = projectileSpeed * std::cos(angleRadians);
            emitterVelocity.y = projectileSpeed * std::sin(angleRadians);
            return fmod(angleDegrees + 90.0, 360.0) - 45*diagonal; // fmod shit because degrees=0 is top right
        }

        inline void projectileVelocityArcGap(const glm::vec2& originVelocity, const float& rotationDegrees, const float& deltaDegrees, glm::vec2& emitterVelocity){
            float deltaRadians = deltaDegrees * (M_PI / 180.0);
            emitterVelocity.x = originVelocity.x * std::cos(deltaRadians) - originVelocity.y * std::sin(deltaRadians);
            emitterVelocity.y = originVelocity.x * std::sin(deltaRadians) + originVelocity.y * std::cos(deltaRadians);
        }

        inline void slimeGodSlow(Entity slimeGod, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            glm::vec2 bigCenter = {slimeGod.GetComponent<TransformComponent>().position.x + 32, slimeGod.GetComponent<TransformComponent>().position.y + 32};
            glm::vec2 velocity;
            constexpr projectilePPD data = {PURPLESTAR};
            float rotationDegrees = getRotationFromCoordiante(448, bigCenter.x, bigCenter.y, playerPos.x+20, playerPos.y+8, velocity, false);
            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
            projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
            projectile.AddComponent<TransformComponent>(bigCenter, glm::vec2(5.0,5.0), rotationDegrees);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.Group(PROJECTILE);
            projectile.AddComponent<RotationComponent>(1);
            projectile.AddComponent<ProjectileComponent>(0, 2000, false, slimeGod, 0, SLIMEGOD, true, SLOWED, 3000, false);
        }

        inline void beholderBlind(Entity beholder, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            glm::vec2 bigCenter = {beholder.GetComponent<TransformComponent>().position.x + 32, beholder.GetComponent<TransformComponent>().position.y + 32};
            glm::vec2 velocity;
            // const auto& sprite = enumToSpriteComponent.at(YELLOWSTAR);
            constexpr projectilePPD data = {YELLOWSTAR};
            float rotationDegrees = getRotationFromCoordiante(448, bigCenter.x, bigCenter.y, playerPos.x+20, playerPos.y+8, velocity, false);
            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
            projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
            projectile.AddComponent<TransformComponent>(bigCenter, glm::vec2(5.0,5.0), rotationDegrees);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.Group(PROJECTILE);
            projectile.AddComponent<RotationComponent>(1);
            projectile.AddComponent<ProjectileComponent>(0, 3000, false, beholder, 0, BEHOLDER, true, BLIND, 3000, false);
        }

        inline void spriteGodBoomerang(Entity spriteGod, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            glm::vec2 bigCenter = {spriteGod.GetComponent<TransformComponent>().position.x + 32, spriteGod.GetComponent<TransformComponent>().position.y + 32};
            glm::vec2 velocity;
            float rotationDegrees = getRotationFromCoordiante(384, bigCenter.x, bigCenter.y, playerPos.x+20, playerPos.y+8, velocity, false);
            Entity projectile = registry->CreateEntity();
            const SDL_Rect whiteBoomerang = {8*3, 8*8,8,8};
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(LOFIOBJ, 8, 8, whiteBoomerang,3,false,false);
            projectile.AddComponent<BoxColliderComponent>(32,32,glm::vec2({4,4}));
            projectile.AddComponent<TransformComponent>(bigCenter, glm::vec2(6.0,6.0), rotationDegrees);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.Group(PROJECTILE);
            projectile.AddComponent<RotationComponent>(1);
            projectile.AddComponent<ProjectileComponent>(0, 2400, false, spriteGod, 0, SPRITEGOD, true, QUIET, 5000, false);
        }

        inline void MedusaBomb(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry, sprites spriteEnum, int damage = 150){
            const auto& transform = monster.GetComponent<TransformComponent>();
            const auto& sprite = monster.GetComponent<SpriteComponent>();
            auto distance = glm::distance(playerPos, transform.position);
            const SDL_Rect redsquare = {0,8*5,8,8};
            Entity square = registry->CreateEntity();
            auto speed = 0.8 * std::exp((std::log(0.625) / 1000) * distance); // ranges between .8 and .6 so distant bombs take longer to arrive
            square.AddComponent<TransformComponent>(transform.center, glm::vec2(5.0,5.0));
            square.AddComponent<SpriteComponent>(LOFIOBJ, 8, 8, redsquare,5,false,false); // experimenting w/ zaxiz of 5 for this                           // height, speed 
            square.AddComponent<ParabolicMovementComponent>(glm::vec3(transform.center.x, transform.center.y, 0.0f), glm::vec3(playerPos.x + 24.0, playerPos.y + 24.0, 0.0f), 120.0, speed, PARABOLIC_MEDUSA_AOE_BOMB);
            square.AddComponent<ProjectileComponent>(damage,INT_MAX,0,monster, 0, spriteEnum); // bogus projectile component needed for damage event logic
        }

        inline void blackTowerBomb(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry, sprites spriteEnum, int damage = 50){
            const auto& transform = monster.GetComponent<TransformComponent>();
            const auto& sprite = monster.GetComponent<SpriteComponent>();
            auto distance = glm::distance(playerPos, transform.position);
            const SDL_Rect blacksquare = {8*2,8*4,8,8};
            Entity square = registry->CreateEntity();
            auto speed = 0.8 * std::exp((std::log(0.625) / 1000) * distance); // ranges between .8 and .6 so distant bombs take longer to arrive
            square.AddComponent<TransformComponent>(transform.center - 20.0f, glm::vec2(5.0,5.0));
            square.AddComponent<SpriteComponent>(LOFIOBJ, 8, 8, blacksquare,5,false,false); // experimenting w/ zaxiz of 5 for this                           // height, speed 
            square.AddComponent<ParabolicMovementComponent>(glm::vec3(transform.center.x - 20.0f, transform.center.y - 20.0f, 0.0f), glm::vec3(playerPos.x + 24.0, playerPos.y + 24.0, 0.0f), 120.0, speed, PARABOLIC_BLACK_AOE_BOMB);
            square.AddComponent<ProjectileComponent>(damage,INT_MAX,false,monster,0, spriteEnum, true, SLOWED, 3000, false); // bogus projectile component needed for damage event logic
        }

        inline void cubeGodShotgun(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            glm::vec2 bigCenter = {monster.GetComponent<TransformComponent>().position.x + 32, monster.GetComponent<TransformComponent>().position.y + 32};
            const SDL_Rect bluebolt = {10*8, 8*11, 8, 8};
            int numshots = 4;
            double realgap = 40 / (numshots-1);
            int speed = 512;
            int duration = 2000;
            bool isDiagonal = true;
            glm::vec2 originVelocity;
            float rotationDegrees = getRotationFromCoordiante(speed, bigCenter.x, bigCenter.y, playerPos.x+20, playerPos.y+8, originVelocity, isDiagonal); // updates originVelocity

            for(int i = 0; i < numshots/2; i++){
                bool first = i != 0;
                Entity projectile = registry->CreateEntity();
                glm::vec2 velocity; // velocity of current projectile
                // problem is probably because projectileVelocityArcGap expects deltaDegrees to be relative to origin
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/2) + realgap*i*first, velocity);
                projectile.AddComponent<RidigBodyComponent>(velocity);
                projectile.AddComponent<SpriteComponent>(LOFIOBJ,8,8,bluebolt,3,false,true);
                projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                projectile.AddComponent<TransformComponent>(bigCenter, glm::vec2(5.0), (rotationDegrees + realgap/2) + realgap*i*first);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.AddComponent<ProjectileComponent>(150, duration, false, monster, 0, CUBEGOD);
                projectile.Group(PROJECTILE);

                Entity projectile2 = registry->CreateEntity();
                glm::vec2 velocity2; // velocity of current projectile
                // problem is probably because projectileVelocityArcGap expects deltaDegrees to be relative to origin
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/-2) + realgap*-i*first, velocity2);
                projectile2.AddComponent<RidigBodyComponent>(velocity2);
                projectile2.AddComponent<SpriteComponent>(LOFIOBJ,8,8,bluebolt,3,false,true);
                projectile2.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                projectile2.AddComponent<TransformComponent>(bigCenter, glm::vec2(5.0), (rotationDegrees - realgap/2) + realgap*-i*first);
                projectile2.AddComponent<LinearProjectileComponent>();
                projectile2.AddComponent<ProjectileComponent>(150, duration, false, monster, 0, CUBEGOD);
                projectile2.Group(PROJECTILE);
            }
        }

        inline void hellgolemshotgun(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& center = monster.GetComponent<TransformComponent>().center;
            constexpr projectilePPD data = {REDMAGIC};
            constexpr int numshots = 19;
            constexpr double realgap = 360 / (numshots - 1);
            constexpr int duration = 300;
            glm::vec2 originVelocity;
            int speed = 300;
            float rotationDegrees = getRotationFromCoordiante(speed, center.x, center.y, playerPos.x+20, playerPos.y+8, originVelocity);
            for(int i = 0; i < numshots/2; i++){
                bool first = i != 0;
                Entity projectile = registry->CreateEntity();
                glm::vec2 velocity; // velocity of current projectile
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/2) + realgap*i*first, velocity);
                projectile.AddComponent<RidigBodyComponent>(velocity);
                projectile.AddComponent<SpriteComponent>(data.texture,3,6,data.rect,3,false,true);
                projectile.AddComponent<BoxColliderComponent>(data.boxWidth,data.boxHeight,data.boxOffset);
                projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees + realgap/2) + realgap*i*first);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.AddComponent<ProjectileComponent>(55, duration, false, monster, 0, HELLGOLEM);
                projectile.Group(PROJECTILE);

                Entity projectile2 = registry->CreateEntity();
                glm::vec2 velocity2; // velocity of current projectile
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/-2) + realgap*-i*first, velocity2);
                projectile2.AddComponent<RidigBodyComponent>(velocity2);
                projectile2.AddComponent<SpriteComponent>(data.texture,3,6,data.rect,3,false,true);
                projectile2.AddComponent<BoxColliderComponent>(data.boxWidth,data.boxHeight,data.boxOffset);
                projectile2.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees - realgap/2) + realgap*-i*first);
                projectile2.AddComponent<LinearProjectileComponent>();
                projectile2.AddComponent<ProjectileComponent>(55, duration, false, monster, 0, HELLGOLEM);
                projectile2.Group(PROJECTILE);
            }        
        }

        inline void henchmanblueshotgun(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& center = monster.GetComponent<TransformComponent>().center;
            constexpr projectilePPD data = {BLUEMAGIC};
            constexpr int numshots = 7;
            constexpr double realgap = 70 / (numshots - 1);
            constexpr int duration = 800;
            glm::vec2 originVelocity;
            int speed = 700;
            for(int j = 0; j < 3; j++){ // shoot 3 shots in each spot with different speeds
                speed -= 100 * j;
                float rotationDegrees = getRotationFromCoordiante(speed, center.x, center.y, playerPos.x+20, playerPos.y+8, originVelocity);
                for(int i = 0; i < numshots/2; i++){
                    bool first = i != 0;
                    Entity projectile = registry->CreateEntity();
                    glm::vec2 velocity; // velocity of current projectile
                    projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/2) + realgap*i*first, velocity);
                    projectile.AddComponent<RidigBodyComponent>(velocity);
                    projectile.AddComponent<SpriteComponent>(data.texture,3,6,data.rect,3,false,true);
                    projectile.AddComponent<BoxColliderComponent>(data.boxWidth,data.boxHeight,data.boxOffset);
                    projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees + realgap/2) + realgap*i*first);
                    projectile.AddComponent<LinearProjectileComponent>();
                    projectile.AddComponent<ProjectileComponent>(55, duration, false, monster, 0, HENCHDEMON0);
                    projectile.Group(PROJECTILE);

                    Entity projectile2 = registry->CreateEntity();
                    glm::vec2 velocity2; // velocity of current projectile
                    projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/-2) + realgap*-i*first, velocity2);
                    projectile2.AddComponent<RidigBodyComponent>(velocity2);
                    projectile2.AddComponent<SpriteComponent>(data.texture,3,6,data.rect,3,false,true);
                    projectile2.AddComponent<BoxColliderComponent>(data.boxWidth,data.boxHeight,data.boxOffset);
                    projectile2.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees - realgap/2) + realgap*-i*first);
                    projectile2.AddComponent<LinearProjectileComponent>();
                    projectile2.AddComponent<ProjectileComponent>(55, duration, false, monster, 0, HENCHDEMON0);
                    projectile2.Group(PROJECTILE);
                }        

            }
        }

        inline void LeviathanShotGun(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& center = monster.GetComponent<TransformComponent>().center;
            constexpr projectilePPD data = {AQUAMISSILE};
            int numshots = 13;
            double realgap = 360 / (numshots-1);
            int speed = 500;
            int duration = 800;
            bool isDiagonal = true;
            glm::vec2 originVelocity;
            float rotationDegrees = getRotationFromCoordiante(speed, center.x, center.y, playerPos.x+20, playerPos.y+8, originVelocity, isDiagonal); // updates originVelocity

            for(int i = 0; i < numshots/2; i++){
                bool first = i != 0;
                Entity projectile = registry->CreateEntity();
                glm::vec2 velocity; // velocity of current projectile
                // problem is probably because projectileVelocityArcGap expects deltaDegrees to be relative to origin
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/2) + realgap*i*first, velocity);
                projectile.AddComponent<RidigBodyComponent>(velocity);
                projectile.AddComponent<SpriteComponent>(data.texture,5,5,data.rect,3,false,true);
                projectile.AddComponent<BoxColliderComponent>(data.boxWidth,data.boxHeight,data.boxOffset);
                projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees + realgap/2) + realgap*i*first);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.AddComponent<ProjectileComponent>(55, duration, false, monster, 0, LEVIATHAN);
                projectile.Group(PROJECTILE);

                Entity projectile2 = registry->CreateEntity();
                glm::vec2 velocity2; // velocity of current projectile
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/-2) + realgap*-i*first, velocity2);
                projectile2.AddComponent<RidigBodyComponent>(velocity2);
                projectile2.AddComponent<SpriteComponent>(data.texture,5,5,data.rect,3,false,true);
                projectile2.AddComponent<BoxColliderComponent>(data.boxWidth,data.boxHeight,data.boxOffset);
                projectile2.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees - realgap/2) + realgap*-i*first);
                projectile2.AddComponent<LinearProjectileComponent>();
                projectile2.AddComponent<ProjectileComponent>(55, duration, false, monster, 0, LEVIATHAN);
                projectile2.Group(PROJECTILE);
            }
        }

        inline void confusedStarShotgeun(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& t = monster.GetComponent<TransformComponent>();
            const auto& s = monster.GetComponent<SpriteComponent>();
            glm::vec2 center = {t.position.x + ((s.height * t.scale.x)/2) - 24, t.position.y + ((s.width * t.scale.y)/2) - 24};
            projectilePPD data = {BLUESTAR};
            int numshots = 4;
            double realgap = 40 / (numshots-1);
            int speed = 512;
            int duration = 2000;
            glm::vec2 originVelocity;
            const auto& spriteEnum = monster.GetComponent<ProjectileEmitterComponent>().emitterSprite;
            float rotationDegrees = getRotationFromCoordiante(speed, center.x, center.y, playerPos.x+20, playerPos.y+8, originVelocity, false); // updates originVelocity
            for(int i = 0; i < numshots/2; i++){
                bool first = i != 0;
                Entity projectile = registry->CreateEntity();
                glm::vec2 velocity; // velocity of current projectile
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/2) + realgap*i*first, velocity);
                projectile.AddComponent<RidigBodyComponent>(velocity);
                projectile.AddComponent<SpriteComponent>(LOFIOBJ,8,8,data.rect,3,false,true);
                projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees + realgap/2) + realgap*i*first);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.AddComponent<ProjectileComponent>(15, duration, false, monster, 0, spriteEnum, true, CONFUSED, 1000, false);
                projectile.AddComponent<RotationComponent>();
                projectile.Group(PROJECTILE);

                Entity projectile2 = registry->CreateEntity();
                glm::vec2 velocity2; // velocity of current projectile
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/-2) + realgap*-i*first, velocity2);
                projectile2.AddComponent<RidigBodyComponent>(velocity2);
                projectile2.AddComponent<SpriteComponent>(LOFIOBJ,8,8,data.rect,3,false,true);
                projectile2.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                projectile2.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees - realgap/2) + realgap*-i*first);
                projectile2.AddComponent<LinearProjectileComponent>();
                projectile2.AddComponent<ProjectileComponent>(15, duration, false, monster, 0, spriteEnum, true, CONFUSED, 1000, false);
                projectile2.AddComponent<RotationComponent>();
                projectile2.Group(PROJECTILE);
            }
        }

        inline void stunedStarShotgun(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& t = monster.GetComponent<TransformComponent>();
            const auto& s = monster.GetComponent<SpriteComponent>();
            glm::vec2 center = {t.position.x + ((s.height * t.scale.x)/2) - 24, t.position.y + ((s.width * t.scale.y)/2) - 24};
            projectilePPD data = {ORANGESTAR};
            int numshots = 4;
            double realgap = 40 / (numshots-1);
            int speed = 512;
            int duration = 2000;
            glm::vec2 originVelocity;
            const auto& spriteEnum = monster.GetComponent<ProjectileEmitterComponent>().emitterSprite;
            float rotationDegrees = getRotationFromCoordiante(speed, center.x, center.y, playerPos.x+20, playerPos.y+8, originVelocity, false); // updates originVelocity
            for(int i = 0; i < numshots/2; i++){
                bool first = i != 0;
                Entity projectile = registry->CreateEntity();
                glm::vec2 velocity; // velocity of current projectile
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/2) + realgap*i*first, velocity);
                projectile.AddComponent<RidigBodyComponent>(velocity);
                projectile.AddComponent<SpriteComponent>(LOFIOBJ,8,8,data.rect,3,false,true);
                projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees + realgap/2) + realgap*i*first);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.AddComponent<ProjectileComponent>(15, duration, false, monster, 0, spriteEnum, true, STUNNED, 1000, false);
                projectile.AddComponent<RotationComponent>();
                projectile.Group(PROJECTILE);

                Entity projectile2 = registry->CreateEntity();
                glm::vec2 velocity2; // velocity of current projectile
                projectileVelocityArcGap(originVelocity, rotationDegrees, (realgap/-2) + realgap*-i*first, velocity2);
                projectile2.AddComponent<RidigBodyComponent>(velocity2);
                projectile2.AddComponent<SpriteComponent>(LOFIOBJ,8,8,data.rect,3,false,true);
                projectile2.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                projectile2.AddComponent<TransformComponent>(center, glm::vec2(5.0), (rotationDegrees - realgap/2) + realgap*-i*first);
                projectile2.AddComponent<LinearProjectileComponent>();
                projectile2.AddComponent<ProjectileComponent>(15, duration, false, monster, 0, spriteEnum, true, STUNNED, 1000, false);
                projectile2.AddComponent<RotationComponent>();
                projectile2.Group(PROJECTILE);
            }
        }

        inline void slowedStar(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& sprite = monster.GetComponent<SpriteComponent>();
            const auto& transform = monster.GetComponent<TransformComponent>();
            glm::vec2 center = {(transform.position.x + ((sprite.width * transform.scale.x) / 2) - 20), 
                                transform.position.y + ((sprite.height * transform.scale.y) / 2) - 20};
            glm::vec2 velocity;
            float rotationDegrees = getRotationFromCoordiante(384, center.x, center.y, playerPos.x+20, playerPos.y+8, velocity, false);
            projectilePPD data = {PURPLESTAR};
            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
            projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
            projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0,5.0));
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.AddComponent<RotationComponent>();
            projectile.AddComponent<ProjectileComponent>(10, 2700, false, monster, 0, CYANCUBE, true, SLOWED, 3000);
            projectile.Group(PROJECTILE);
        }   

        inline void paralyzedStar(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& sprite = monster.GetComponent<SpriteComponent>();
            const auto& transform = monster.GetComponent<TransformComponent>();
            glm::vec2 center = {(transform.position.x + ((sprite.width * transform.scale.x) / 2) - 20), 
                                transform.position.y + ((sprite.height * transform.scale.y) / 2) - 20};
            glm::vec2 velocity;
            float rotationDegrees = getRotationFromCoordiante(500, center.x, center.y, playerPos.x+20, playerPos.y+8, velocity, false);
            projectilePPD data = {BLACKSTAR};
            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
            projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
            projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0,5.0));
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.AddComponent<RotationComponent>();
            projectile.AddComponent<ProjectileComponent>(15, 2700, false, monster, 0, SHADE, true, PARALYZE, 2000);
            projectile.Group(PROJECTILE);
        }  

        inline void chicken(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& sprite = monster.GetComponent<SpriteComponent>();
            const auto& transform = monster.GetComponent<TransformComponent>();
            glm::vec2 center = {(transform.position.x + ((sprite.width * transform.scale.x) / 2) - 20), 
                                transform.position.y + ((sprite.height * transform.scale.y) / 2) - 20};
            glm::vec2 velocity;
            float rotationDegrees = getRotationFromCoordiante(400, center.x, center.y, playerPos.x+20, playerPos.y+8, velocity, false);
            projectilePPD data = {CHICKENBOLT};
            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
            projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
            projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0,5.0));
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.AddComponent<RotationComponent>();
            projectile.AddComponent<ProjectileComponent>(30, 2700, false, monster, 0, BOSSCHICKEN, false);
            projectile.Group(PROJECTILE);
        }

        inline void fireBolt(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& sprite = monster.GetComponent<SpriteComponent>();
            const auto& transform = monster.GetComponent<TransformComponent>();
            glm::vec2 center = {(transform.position.x + ((sprite.width * transform.scale.x) / 2) - 20), 
                                transform.position.y + ((sprite.height * transform.scale.y) / 2) - 20};
            glm::vec2 velocity;
            float rotationDegrees = getRotationFromCoordiante(512, center.x, center.y, playerPos.x+20, playerPos.y+8, velocity, true);
            projectilePPD data = {REDBOLT};
            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
            projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
            projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0,5.0), rotationDegrees);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.AddComponent<ProjectileComponent>(100, 2500, false, monster, 0, ORANGECUBE);
            projectile.Group(PROJECTILE);
        }  

        inline void bigArrow(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            const auto& sprite = monster.GetComponent<SpriteComponent>();
            const auto& transform = monster.GetComponent<TransformComponent>();
            glm::vec2 center = {(transform.position.x + ((sprite.width * transform.scale.x) / 2) - 20), 
                                transform.position.y + ((sprite.height * transform.scale.y) / 2) - 20};
            glm::vec2 velocity;
            float rotationDegrees = getRotationFromCoordiante(512, center.x, center.y, playerPos.x+20, playerPos.y+8, velocity, true);
            projectilePPD data = {BLACKARROW};
            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
            projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
            projectile.AddComponent<TransformComponent>(center, glm::vec2(5.0,5.0), rotationDegrees);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.AddComponent<ProjectileComponent>(100, 2500, false, monster, 0, GHOSTARCHER, true, SLOWED, 3000);
            projectile.Group(PROJECTILE);
        }

    public:
        SecondaryProjectileEmitSystem();
        void Update(const glm::vec2& playerPos, std::unique_ptr<Registry>& registry);

};

#endif