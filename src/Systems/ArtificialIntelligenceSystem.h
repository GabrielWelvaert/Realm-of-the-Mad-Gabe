#ifndef ARTIFICIALINTELLIGENCESYSTEM_H
#define ARTIFICIALINTELLIGENCESYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/glm/glm.hpp"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/AnimatedShootingComponent.h"
#include "../../libs/SDL2/SDL.h"
#include <vector>
#include "../Components/PassiveAIComponent.h"
#include "../Components/ChaseAIComponent.h"
#include "../Components/NeutralAIComponent.h"
#include "../Components/TrapAIComponent.h"
#include "../Components/AnimatedChaseAIComponent.h"
#include "../Components/AnimatedNeutralAIComponent.h"
#include "../Components/AnimationComponent.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/StatusEffectComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/HPMPComponent.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/BossAIComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Utils/factory.h"
#include "../Utils/roomShut.h"
#include "../Utils/room.h"
#include "../Components/LinearProjectileComponent.h"
#include "../Components/OscillatingProjectileComponent.h"
#include "../Components/isShootingComponent.h"
#include "../Components/MinionComponent.h"
#include "../../libs/glm/gtc/random.hpp"

/*
These systems are like the KBMS but for monsters; they update sprite-atlas ranges, velocities, and various flags based off of their reaction environmental (player) conditions
*/

// never shoots or chases.
class PassiveAISystem: public System{
    public:
        
        PassiveAISystem();

        void Update(const Entity& player);
};

// chases and shooots
class ChaseAISystem: public System{
    public:

        ChaseAISystem();

        void Update(const Entity& player);
};

// shoots, does not chase
class NeutralAISystem: public System{
    public:

        NeutralAISystem();
        void Update(const Entity& player);
};

class TrapAISystem: public System{
    public:

        TrapAISystem();

        void Update(const Entity& player, std::unique_ptr<AssetStore>& assetStore);
};

// chases and shooots (animated)
class AnimatedChaseAISystem: public System{
    public:

        AnimatedChaseAISystem();

        void Update(const Entity& player);
};

// shoots, does not chase (animated)
class AnimatedNeutralAISystem: public System{
    public:

        AnimatedNeutralAISystem();

        void Update(const Entity& player);
};

class MinionAISystem: public System{
    public:
        MinionAISystem();
        void Update(const Entity& player, std::unique_ptr<Registry>& registry);
};

class BossAISystem: public System{
    private:
        Xoshiro256 RNG;

        const int tileScale = 64;

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

        // hard-coded to work for gordon. must update to call with different boss!
        inline void starShotgun(Entity boss, std::unique_ptr<Registry>& registry, const glm::vec2& playerPos, bool longRange = false){ // hey look! PEC::Update() in a method! 
            std::vector<statuses> debuffs = {BLEEDING, QUIET, CONFUSED, QUIET, SLOWED}; // [1] used for armor piercing shot
            std::vector<sprites> stars = {REDSTAR, WHITESTAR, BLUESTAR, GREENSTAR, PURPLESTAR};
            std::vector<glm::vec2> velocities = {glm::vec2(0.0),glm::vec2(0.0),glm::vec2(0.0),glm::vec2(0.0),glm::vec2(0.0)};
            glm::vec2 bossCenter = {boss.GetComponent<TransformComponent>().position.x + 32, boss.GetComponent<TransformComponent>().position.y + 32};
            float rotationDegrees = getRotationFromCoordiante(750, bossCenter.x, bossCenter.y, playerPos.x+20, playerPos.y+8, velocities[2], false);
            constexpr int realgap = 120 / 5;
            int duration;
            longRange ? duration = 1250 : duration = 500;
            for(int i = 0; i < 5; i++){ 
                int j = RNG.randomFromRange(0,4);
                const auto& sprite = enumToSpriteComponent.at(stars[j]);
                if(i != 2){ // do for all except shot at origin
                    projectileVelocityArcGap(velocities[2], rotationDegrees, realgap*(i-2), velocities[i]);
                }
                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<RidigBodyComponent>(velocities[i]);
                projectile.AddComponent<SpriteComponent>(sprite.assetId, sprite.width, sprite.height, sprite.srcRect, sprite.zIndex, sprite.isFixed, sprite.diagonalSprite);
                projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(5.0,5.0), rotationDegrees + realgap*i-2);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.Group(PROJECTILE);
                switch(stars[j]){
                    case REDSTAR:{
                        projectile.AddComponent<ProjectileComponent>(25, duration, false, boss, 0, GORDON, true, BLEEDING, 3000, false);         
                    } break;
                    case WHITESTAR:{
                        projectile.AddComponent<ProjectileComponent>(25, duration, false, boss, 0, GORDON, false, BLEEDING, 5, true); 
                    } break;
                    case BLUESTAR:{
                        projectile.AddComponent<ProjectileComponent>(25, duration, false, boss, 0, GORDON, true, CONFUSED, 3000, false);
                    } break;
                    case GREENSTAR:{ // QUIET
                        projectile.AddComponent<ProjectileComponent>(25, duration, false, boss, 0, GORDON, true, QUIET, 3000, false);
                    } break;
                    case PURPLESTAR:{ /// SLOWED
                        projectile.AddComponent<ProjectileComponent>(25, duration, false, boss, 0, GORDON, true, SLOWED, 3000, false);
                    } break;
                    // todo add stunned projectile
                }
            }
        }

        inline void gordonSurvivalShotGun(Entity boss, std::unique_ptr<Registry>& registry, const glm::vec2& playerPos){
            std::vector<glm::vec2> velocities = {glm::vec2(0.0),glm::vec2(0.0),glm::vec2(0.0)};
            glm::vec2 bossCenter = {boss.GetComponent<TransformComponent>().position.x + 32, boss.GetComponent<TransformComponent>().position.y + 32};
            float rotationDegrees = getRotationFromCoordiante(750, bossCenter.x, bossCenter.y, playerPos.x+20, playerPos.y+8, velocities[1], false);
            constexpr int realgap = 120 / 3;
            const auto& sprite = enumToSpriteComponent.at(ORYXARROW);
            for(int i = 0; i < 3; i++){
                if(i != 1){ 
                    projectileVelocityArcGap(velocities[1], rotationDegrees, realgap*(i-1), velocities[i]);
                }
                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<RidigBodyComponent>(velocities[i]);
                projectile.AddComponent<SpriteComponent>(sprite.assetId, sprite.width, sprite.height, sprite.srcRect, sprite.zIndex, sprite.isFixed, sprite.diagonalSprite);
                projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(6.0,6.0), rotationDegrees + realgap*(i-1));
                projectile.Group(PROJECTILE);
                projectile.AddComponent<ProjectileComponent>(100, 500, false, boss, 0, GORDON, false, SLOWED, 3000, true);
                projectile.AddComponent<LinearProjectileComponent>();
            }

            velocities = {glm::vec2(0.0),glm::vec2(0.0),glm::vec2(0.0)};
            rotationDegrees = getRotationFromCoordiante(500, bossCenter.x, bossCenter.y, playerPos.x+20, playerPos.y+8, velocities[1], false);
            for(int i = 0; i < 3; i++){
                if(i != 1){
                    projectileVelocityArcGap(velocities[1], rotationDegrees, realgap*(i-1), velocities[i]);
                }
                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<RidigBodyComponent>(velocities[i]);
                projectile.AddComponent<SpriteComponent>(sprite.assetId, sprite.width, sprite.height, sprite.srcRect, sprite.zIndex, sprite.isFixed, sprite.diagonalSprite);
                projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(6.0,6.0), rotationDegrees + realgap*(i-1));
                projectile.Group(PROJECTILE);
                projectile.AddComponent<ProjectileComponent>(100, 500, false, boss, 0, GORDON, false, SLOWED, 3000, true);
                projectile.AddComponent<LinearProjectileComponent>();
            }
        }

        inline void gordonPhaseTwoShots(Entity boss, std::unique_ptr<Registry>& registry, int& startIndex, const std::vector<glm::vec2>& positions){
            int arcgap = 350;
            int realgap = arcgap / 3;
            glm::vec2 bossCenter = {boss.GetComponent<TransformComponent>().position.x + 32, boss.GetComponent<TransformComponent>().position.y + 32};
            const auto& sprite = enumToSpriteComponent.at(ORYXTEAR);
            for(int i = 0; i < 6; i++){
                arcgap += 5;
                int realgap = arcgap / 3;
                std::vector<glm::vec2> velocities = {glm::vec2(0.0),glm::vec2(0.0),glm::vec2(0.0)};
                if(startIndex > 35){
                    startIndex = 0;
                }
                glm::vec2 target = positions[startIndex];
                startIndex++;
                float rotationDegrees = getRotationFromCoordiante(250, bossCenter.x, bossCenter.y, target.x, target.y, velocities[0], true);
                for(int i = 0; i < 3; i++){
                    Entity projectile = registry->CreateEntity();
                    if(i != 0){ // do for all except shot at origin
                        projectileVelocityArcGap(velocities[0], rotationDegrees, realgap*i, velocities[i]);
                    }
                    projectile.AddComponent<RidigBodyComponent>(velocities[i]);
                    projectile.AddComponent<SpriteComponent>(sprite.assetId, sprite.width, sprite.height, sprite.srcRect, sprite.zIndex, sprite.isFixed, sprite.diagonalSprite);
                    projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2({14,14}));
                    projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(5.0,5.0), rotationDegrees + realgap*i);
                    projectile.AddComponent<ProjectileComponent>(35, 2075, false, boss, 0, GORDON, false, SLOWED, 3000, false);
                    projectile.AddComponent<LinearProjectileComponent>();
                    projectile.Group(PROJECTILE);
                }
            }
        }

        inline void arcMageConfuseShots(Entity boss, glm::vec2& spawnpoint, std::unique_ptr<Registry>& registry, std::vector<glm::vec2>& phaseOnePositions){
            glm::vec2 originVelocity;
            for(const auto& pos: phaseOnePositions){
                float rotationDegrees = getRotationFromCoordiante(512, spawnpoint.x, spawnpoint.y, pos.x, pos.y, originVelocity, false);
                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<RidigBodyComponent>(originVelocity);
                SDL_Rect rect = {8*3,8*6,8,8};
                projectile.AddComponent<SpriteComponent>(LOFIOBJ,8,8,rect,4,false,false);
                projectile.AddComponent<BoxColliderComponent>(10,10,glm::vec2(14,14));
                projectile.AddComponent<TransformComponent>(glm::vec2(spawnpoint.x+32, spawnpoint.y+32), glm::vec2(5.0,5.0), rotationDegrees);
                projectile.AddComponent<ProjectileComponent>(15, 5000, false, boss, 0, ARCMAGE, true, CONFUSED, 5000, true);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.Group(PROJECTILE);
            }
        }

        inline void arcMageWallShots(Entity boss, std::unique_ptr<Registry>& registry, const room& br){
            glm::vec2 originVelocity;
            int leftHole = RNG.randomFromRange(0,13);
            int rightHole = RNG.randomFromRange(0,13);
            for(int i = 0; i < 15; i++){
                int ypos = (br.y + i) * tileScale + 8;
                for(int j = 0; j < 2; j++){
                    if( (leftHole == i && j == 0) || (rightHole == i && j == 1) ){
                        continue; // dont emit this shot to create hole where player can walk
                    }
                    int xpos = (br.x * tileScale) + ((br.w - 2) * tileScale * j); 
                    float rotationDegrees = getRotationFromCoordiante(128, xpos, ypos , xpos + (1*tileScale - (j * (tileScale*2))) , ypos, originVelocity, false);
                    Entity projectile = registry->CreateEntity();
                    projectile.AddComponent<RidigBodyComponent>(originVelocity);
                    SDL_Rect rect = {8*1,8*22,8,8};
                    projectile.AddComponent<SpriteComponent>(LOFIOBJ3,8,8,rect,4,false,false);
                    projectile.AddComponent<BoxColliderComponent>(32,32,glm::vec2(4,4));
                    projectile.AddComponent<TransformComponent>(glm::vec2(xpos , ypos), glm::vec2(5.0,5.0), rotationDegrees);
                    projectile.AddComponent<ProjectileComponent>(55, 20000, false, boss, 0, ARCMAGE, false, QUIET, 0, false);
                    projectile.AddComponent<LinearProjectileComponent>();
                    projectile.Group(PROJECTILE);
                }
            }
        }

        inline void arcMageSpawnMinionsPhaseTwo(std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory, const room& br){
            if(registry->getNumberOfLivingEntities() > 1000){return;}
            int numToSpawn = RNG.randomFromRange(5,8);
            sprites monster;
            for(int i = 0; i <= numToSpawn; i++){
                float xpos = (RNG.randomFromRange(br.x + 4,  br.x + br.w - 4) * 64);
                float ypos = (RNG.randomFromRange(br.y + 3,  br.y + br.h - 3) * 64);    
                i % 2 == 0 ? monster = SHATTERSBOMB : monster = BAT0;
                factory->spawnMonster(registry, {xpos,ypos}, monster);
            }
        }

        inline void arcMageSpawnMinionsSurvival(std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory, const room& br){
            if(registry->getNumberOfLivingEntities() > 1000){return;}
            int numToSpawn = RNG.randomFromRange(3,5);
            sprites monster;
            for(int i = 0; i <= numToSpawn; i++){
                float xpos = (RNG.randomFromRange(br.x + 2,  br.x + br.w - 2) * 64);
                float ypos = (RNG.randomFromRange(br.y + 2,  br.y + br.h - 2) * 64);    
                i % 2 == 0 ? monster = SHATTERSBOMB : monster = BAT0;
                factory->spawnMonster(registry, {xpos,ypos}, monster);
            }
        }

    public:
        BossAISystem();
        void Update(const Entity& player, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory, roomShut& roomToShut, const SDL_Rect& camera, const room& bossRoom);
};

// class AnimatedPounceAISystem: public System{
//     public:
//         AnimatedPounceAISystem();
//         void Update(const glm::vec2& playerPos);
// };
#endif 