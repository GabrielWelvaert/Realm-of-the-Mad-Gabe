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

/*
These systems are like the KBMS but for monsters; they update sprite-atlas ranges, velocities, and various flags based off of their reaction environmental (player) conditions
*/

// never shoots or chases.
class PassiveAISystem: public System{
    public:
        
        PassiveAISystem();

        void Update(const glm::vec2& playerPos);
};

// chases and shooots
class ChaseAISystem: public System{
    public:

        ChaseAISystem();

        void Update(const glm::vec2& playerPos);
};

// shoots, does not chase
class NeutralAISystem: public System{
    public:

        NeutralAISystem();
        void Update(const glm::vec2& playerPos);
};

class TrapAISystem: public System{
    public:

        TrapAISystem();

        void Update(const glm::vec2& playerPos, std::unique_ptr<AssetStore>& assetStore);
};

// chases and shooots (animated)
class AnimatedChaseAISystem: public System{
    public:

        AnimatedChaseAISystem();

        void Update(const glm::vec2& playerPos);
};

// shoots, does not chase (animated)
class AnimatedNeutralAISystem: public System{
    public:

        AnimatedNeutralAISystem();

        void Update(const glm::vec2& playerPos);
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
            int numToSpawn = RNG.randomFromRange(2,5);
            sprites monster;
            for(int i = 0; i <= numToSpawn; i++){
                float xpos = (RNG.randomFromRange(br.x + 1,  br.x + br.w - 1) * 64);
                float ypos = (RNG.randomFromRange(br.y + 1,  br.y + br.h - 1) * 64);    
                i % 2 == 0 ? monster = SHATTERSBOMB : monster = BAT0;
                factory->spawnMonster(registry, {xpos,ypos}, monster);
            }
        }

    public:
        BossAISystem();
        void Update(const glm::vec2& playerPos, std::unique_ptr<AssetStore>& assetStore, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory, roomShut& roomToShut, const SDL_Rect& camera, const room& bossRoom);
};

class AnimatedPounceAISystem: public System{
    public:
        AnimatedPounceAISystem();
        void Update(const glm::vec2& playerPos);
};
#endif 