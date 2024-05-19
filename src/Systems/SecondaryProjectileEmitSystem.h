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

class SecondaryProjectileEmitSystem: public System{
    private:
        // const int tileScale = 64;
        // const float gravity = 9.81f;

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
            const auto& sprite = enumToSpriteComponent.at(PURPLESTAR);
            float rotationDegrees = getRotationFromCoordiante(448, bigCenter.x, bigCenter.y, playerPos.x+20, playerPos.y+8, velocity, false);
            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(sprite.assetId, sprite.width, sprite.height, sprite.srcRect, sprite.zIndex, sprite.isFixed, sprite.diagonalSprite);
            projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
            projectile.AddComponent<TransformComponent>(bigCenter, glm::vec2(5.0,5.0), rotationDegrees);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.Group(PROJECTILE);
            projectile.AddComponent<RotationComponent>(1);
            projectile.AddComponent<ProjectileComponent>(0, 2000, false, slimeGod, 0, SLIMEGOD, true, SLOWED, 3000, false);
        }

        inline void beholderBlind(Entity beholder, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            glm::vec2 bigCenter = {beholder.GetComponent<TransformComponent>().position.x + 32, beholder.GetComponent<TransformComponent>().position.y + 32};
            glm::vec2 velocity;
            const auto& sprite = enumToSpriteComponent.at(YELLOWSTAR);
            float rotationDegrees = getRotationFromCoordiante(448, bigCenter.x, bigCenter.y, playerPos.x+20, playerPos.y+8, velocity, false);
            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(sprite.assetId, sprite.width, sprite.height, sprite.srcRect, sprite.zIndex, sprite.isFixed, sprite.diagonalSprite);
            projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
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

        inline void MedusaBomb(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            glm::vec2 bigCenter = {monster.GetComponent<TransformComponent>().position.x + 32, monster.GetComponent<TransformComponent>().position.y + 32};
            const SDL_Rect redsquare = {0,8*5,8,8};
            Entity square = registry->CreateEntity();
            square.AddComponent<TransformComponent>(bigCenter, glm::vec2(5.0,5.0));
            square.AddComponent<SpriteComponent>(LOFIOBJ, 8, 8, redsquare,3,false,false);                                                                       // height, speed
            square.AddComponent<ParabolicMovementComponent>(glm::vec3(bigCenter.x, bigCenter.y, 0.0f), glm::vec3(playerPos.x + 24.0, playerPos.y + 24.0, 0.0f), 120.0, .8, PARABOLIC_MEDUSA_AOE_BOMB);
            square.AddComponent<ProjectileComponent>(150,INT_MAX,0,monster, 0,MEDUSA); // bogus projectile component needed for damage event logic
        }

        inline void cubeGodShotgun(Entity monster, const glm::vec2& playerPos, std::unique_ptr<Registry>& registry){
            glm::vec2 bigCenter = {monster.GetComponent<TransformComponent>().position.x + 32, monster.GetComponent<TransformComponent>().position.y + 32};
            const SDL_Rect bluebolt = {10*8, 8*11, 8, 8};
            Entity projectile = registry->CreateEntity();
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

    public:
        SecondaryProjectileEmitSystem();
        void Update(const glm::vec2& playerPos, std::unique_ptr<Registry>& registry);

};

#endif