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
#include "../Utils/RoomShut.h"
#include "../Utils/room.h"
#include "../Components/LinearProjectileComponent.h"
#include "../Components/OscillatingProjectileComponent.h"
#include "../Components/isShootingComponent.h"
#include "../Components/MinionComponent.h"
#include "../../libs/glm/gtc/random.hpp"
#include "../Utils/tables.h"
#include "../Components/RotationComponent.h"
#include "../Components/OrbitShootMinionComponent.h"
#include "../Components/StandShootMinionComponent.h"
#include "../Components/TowerComponent.h"
#include "../Components/InvisibleBossComponent.h"
#include "../Components/RandomChaseMinionComponent.h"

#define PENTARACT_TOWER_DISTANCE = 300

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

        void Update(const Entity& player, const glm::vec2& playerPos);
};

// shoots, does not chase
class NeutralAISystem: public System{
    public:

        NeutralAISystem();
        void Update(const Entity& player, const glm::vec2& playerPos);
};

class TrapAISystem: public System{
    public:

        TrapAISystem();

        void Update(const Entity& player, std::unique_ptr<AssetStore>& assetStore, const glm::vec2& playerPos);
};

// chases and shooots (animated)
class AnimatedChaseAISystem: public System{
    public:

        AnimatedChaseAISystem();

        void Update(const Entity& player, const glm::vec2& playerPos);
};

// shoots, does not chase (animated)
class AnimatedNeutralAISystem: public System{
    public:

        AnimatedNeutralAISystem();

        void Update(const Entity& player, const glm::vec2& playerPos);
};

class OrbitMinionAISystem: public System{
    private:
        Xoshiro256 RNG;
    public:
        OrbitMinionAISystem();
        void Update(const Entity& player, std::unique_ptr<Registry>& registry, const glm::vec2& playerPos);
};

class OrbitShootMinionAISystem: public System{
    public:
        OrbitShootMinionAISystem();
        void Update(const Entity& player, std::unique_ptr<Registry>& registry, const glm::vec2& playerPos);

};

class StandShootMinionAISystem: public System{
    public:
        StandShootMinionAISystem();
        void Update(const Entity& player, std::unique_ptr<Registry>& registry, const glm::vec2& playerPos);
};

class randomChaseMinionAISystem: public System{
    private:
        Xoshiro256 RNG;
        inline glm::vec2 spriteCenter(const TransformComponent& t, const SpriteComponent& s){
            return {(t.position.x + ((s.width * t.scale.x) / 2)), t.position.y + ((s.height * t.scale.y) / 2)};
        }

    public:
        randomChaseMinionAISystem();
        void Update(const Entity& player, std::unique_ptr<Registry>& registry, const glm::vec2& playerPos);

};

class InvisibleBossAISystem: public System{
    private:
        Xoshiro256 RNG;

        inline float angleBetweenTwoPoints(const glm::vec2& origin, const glm::vec2& dest){
            float angleRadians = std::atan2(dest.y - origin.y, dest.x - origin.x);   
            float angleDegrees = angleRadians * (180.0 / M_PI);
            return angleDegrees;
        }

        inline bool floatCompare(float a, float b, float tolerance){
            return std::fabs(a - b) <= tolerance;
        }

        inline glm::vec2 towerDestination(const glm::vec2& origin, float angleDegrees, float distance){
            float angleRadians = glm::radians(angleDegrees);
            float deltaX = distance * std::cos(angleRadians);
            float deltaY = distance * std::sin(angleRadians);
            glm::vec2 destination = origin + glm::vec2(deltaX, deltaY);
            return destination;
        }

    public: 
        InvisibleBossAISystem();
        void Update(const Entity& player, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory, const room& bossRoom, const glm::vec2& playerPos);

};

class BossAISystem: public System{
    private:

        inline glm::vec2 spriteCenter(const TransformComponent& t, const SpriteComponent& s){
            return {(t.position.x + ((s.width * t.scale.x) / 2)), t.position.y + ((s.height * t.scale.y) / 2)};
        }

        inline bool floatCompare(float a, float b, float tolerance){
            return std::fabs(a - b) <= tolerance;
        }
        
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
            std::vector<projectilePPD> stars = {{REDSTAR}, {WHITESTAR}, {BLUESTAR}, {GREENSTAR}, {PURPLESTAR}};
            std::vector<glm::vec2> velocities = {glm::vec2(0.0),glm::vec2(0.0),glm::vec2(0.0),glm::vec2(0.0),glm::vec2(0.0)};
            glm::vec2 bossCenter = {boss.GetComponent<TransformComponent>().position.x + 32, boss.GetComponent<TransformComponent>().position.y + 32};
            float rotationDegrees = getRotationFromCoordiante(750, bossCenter.x, bossCenter.y, playerPos.x+20, playerPos.y+8, velocities[2], false);
            constexpr int realgap = 120 / 5;
            int duration;
            longRange ? duration = 1250 : duration = 500;
            for(int i = 0; i < 5; i++){ 
                int j = RNG.randomFromRange(0,4);
                projectilePPD data = stars[j];
                if(i != 2){ // do for all except shot at origin
                    projectileVelocityArcGap(velocities[2], rotationDegrees, realgap*(i-2), velocities[i]);
                }
                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<RidigBodyComponent>(velocities[i]);
                projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
                projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
                projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(5.0,5.0), rotationDegrees + realgap*i-2);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.AddComponent<RotationComponent>();
                projectile.Group(PROJECTILE);
                switch(j){
                    case 0:{
                        projectile.AddComponent<ProjectileComponent>(25, duration, false, boss, 0, GORDON, true, BLEEDING, 3000, false);         
                    } break;
                    case 1:{
                        projectile.AddComponent<ProjectileComponent>(25, duration, false, boss, 0, GORDON, false, BLEEDING, 5, true); 
                    } break;
                    case 2:{
                        projectile.AddComponent<ProjectileComponent>(25, duration, false, boss, 0, GORDON, true, CONFUSED, 3000, false);
                    } break;
                    case 3:{ // QUIET
                        projectile.AddComponent<ProjectileComponent>(25, duration, false, boss, 0, GORDON, true, QUIET, 3000, false);
                    } break;
                    case 4:{ /// SLOWED
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
            projectilePPD data = {ORYXARROW};
            for(int i = 0; i < 3; i++){
                if(i != 1){ 
                    projectileVelocityArcGap(velocities[1], rotationDegrees, realgap*(i-1), velocities[i]);
                }
                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<RidigBodyComponent>(velocities[i]);
                projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
                projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
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
                projectile.AddComponent<SpriteComponent>(data.texture, data.rect);
                projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
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
            constexpr projectilePPD data = {ORYXTEAR};
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
                    projectile.AddComponent<SpriteComponent>(data.texture, data.rect, true);
                    projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
                    projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(5.0,5.0), rotationDegrees + realgap*i);
                    projectile.AddComponent<ProjectileComponent>(35, 2075, false, boss, 0, GORDON, false, SLOWED, 3000, false);
                    projectile.AddComponent<LinearProjectileComponent>();
                    projectile.Group(PROJECTILE);
                }
            }
        }

        inline void spiralBalls(Entity boss, std::unique_ptr<Registry>& registry, const glm::vec2& target){
            float gap = 90.0f;
            glm::vec2 bossCenter = {boss.GetComponent<TransformComponent>().position.x + 32, boss.GetComponent<TransformComponent>().position.y + 32};
            constexpr projectilePPD data = {ORYXCIRCLE};
            glm::vec2 ov; // origin velocity
            float rotationDegrees = getRotationFromCoordiante(512, bossCenter.x, bossCenter.y, target.x, target.y, ov);
            for(int i = 0; i < 4; i++){
                glm::vec2 velocity;
                Entity projectile = registry->CreateEntity();
                projectileVelocityArcGap(ov, rotationDegrees, gap*i, velocity);
                projectile.AddComponent<RidigBodyComponent>(velocity);
                projectile.AddComponent<SpriteComponent>(data.texture, data.rect, true);
                projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
                projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(5.0,5.0), rotationDegrees + gap*i);
                projectile.AddComponent<ProjectileComponent>(95, 1500, false, boss, 0, MYSTERIOUSCRYSTAL);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.Group(PROJECTILE);
            }
        }

        inline void revolvingSlowShotgun(Entity boss, std::unique_ptr<Registry>& registry, const glm::vec2& target){
            float gap = 120.0f;
            glm::vec2 bossCenter = {boss.GetComponent<TransformComponent>().position.x + 32, boss.GetComponent<TransformComponent>().position.y + 32};
            constexpr projectilePPD data = {PURPLESTAR};
            glm::vec2 ov; // origin velocity
            float rotationDegrees = getRotationFromCoordiante(400, bossCenter.x, bossCenter.y, target.x, target.y, ov);
            for(int i = 0; i < 3; i++){
                glm::vec2 velocity;
                Entity projectile = registry->CreateEntity();
                projectileVelocityArcGap(ov, rotationDegrees, gap*i, velocity);
                projectile.AddComponent<RidigBodyComponent>(velocity);
                projectile.AddComponent<SpriteComponent>(data.texture, data.rect, true);
                projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
                projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(5.0,5.0), rotationDegrees + gap*i);
                projectile.AddComponent<ProjectileComponent>(50, 1500, false, boss, 0, MYSTERIOUSCRYSTAL, true, SLOWED, 3000);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.AddComponent<RotationComponent>();
                projectile.Group(PROJECTILE);
            }
        }

        inline void revolvingConfusedShots(Entity boss, std::unique_ptr<Registry>& registry, const glm::vec2& target){
            float gap = 60.0f;
            const auto& transform = boss.GetComponent<TransformComponent>();
            const auto& sprite = boss.GetComponent<SpriteComponent>();
            glm::vec2 bossCenter = {transform.position.x + (sprite.width * transform.scale.x)/2.0f, transform.position.y + (sprite.height * transform.scale.y)/2.0f};
            constexpr projectilePPD data = {BLUESTAR};
            glm::vec2 ov; // origin velocity
            float rotationDegrees = getRotationFromCoordiante(400, bossCenter.x, bossCenter.y, target.x, target.y, ov);
            for(int i = 0; i < 6; i++){
                glm::vec2 velocity;
                Entity projectile = registry->CreateEntity();
                projectileVelocityArcGap(ov, rotationDegrees, gap*i, velocity);
                projectile.AddComponent<RidigBodyComponent>(velocity);
                projectile.AddComponent<SpriteComponent>(data.texture, data.rect, true);
                projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
                projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(5.0,5.0), rotationDegrees + gap*i);
                projectile.AddComponent<ProjectileComponent>(35, 1500, false, boss, 0, MYSTERIOUSCRYSTAL, true, CONFUSED, 3000);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.AddComponent<RotationComponent>();
                projectile.Group(PROJECTILE);
            }
        }

        inline void zShotguns(Entity boss, std::unique_ptr<Registry>& registry, const glm::vec2& target){
            float gap = 10.0f;
            const auto& transform = boss.GetComponent<TransformComponent>();
            const auto& sprite = boss.GetComponent<SpriteComponent>();
            glm::vec2 bossCenter = {transform.position.x + (sprite.width * transform.scale.x)/2.0f, transform.position.y + (sprite.height * transform.scale.y)/2.0f};
            constexpr projectilePPD data = {ZSHOT};
            glm::vec2 ov; // origin velocity
            float rotationDegrees = getRotationFromCoordiante(352, bossCenter.x, bossCenter.y, target.x, target.y, ov);
            for(int i = 0; i < 9; i++){
                glm::vec2 velocity;
                Entity projectile = registry->CreateEntity();
                projectileVelocityArcGap(ov, rotationDegrees, gap*i, velocity);
                projectile.AddComponent<RidigBodyComponent>(velocity);
                projectile.AddComponent<SpriteComponent>(data.texture, data.rect, true);
                projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
                projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(5.0,5.0), rotationDegrees + gap*i);
                projectile.AddComponent<ProjectileComponent>(150, 1900, false, boss, 0, GRANDSPHINX, true, BLIND, 3000);
                projectile.AddComponent<LinearProjectileComponent>();
                projectile.AddComponent<RotationComponent>();
                projectile.Group(PROJECTILE);
            }
        }
        
        inline void fireBall(Entity boss, std::unique_ptr<Registry>& registry, const glm::vec2& target){
            const auto& transform = boss.GetComponent<TransformComponent>();
            const auto& sprite = boss.GetComponent<SpriteComponent>();
            glm::vec2 bossCenter = {transform.position.x + (sprite.width * transform.scale.x)/2.0f, transform.position.y + (sprite.height * transform.scale.y)/2.0f};
            constexpr projectilePPD data = {FIREBALL};
            glm::vec2 ov; // origin velocity
            float rotationDegrees = getRotationFromCoordiante(352, bossCenter.x, bossCenter.y, target.x, target.y, ov);
            glm::vec2 velocity;
            Entity projectile = registry->CreateEntity();
            projectileVelocityArcGap(ov, rotationDegrees, 0.0f, velocity);
            projectile.AddComponent<RidigBodyComponent>(velocity);
            projectile.AddComponent<SpriteComponent>(data.texture, data.rect, true);
            projectile.AddComponent<BoxColliderComponent>(data.boxWidth, data.boxHeight, data.boxOffset);
            projectile.AddComponent<TransformComponent>(bossCenter, glm::vec2(5.0,5.0), rotationDegrees);
            projectile.AddComponent<ProjectileComponent>(150, 1900, false, boss, 0, GRANDSPHINX, true, BLIND, 3000);
            projectile.AddComponent<LinearProjectileComponent>();
            projectile.AddComponent<RotationComponent>();
            projectile.Group(PROJECTILE);
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
                projectile.AddComponent<RotationComponent>();
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
        void Update(const Entity& player, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory, RoomShut& roomToShut, const SDL_Rect& camera, const room& bossRoom, const glm::vec2& playerPos);
};

// class AnimatedPounceAISystem: public System{
//     public:
//         AnimatedPounceAISystem();
//         void Update(const glm::vec2& playerPos);
// };
#endif 