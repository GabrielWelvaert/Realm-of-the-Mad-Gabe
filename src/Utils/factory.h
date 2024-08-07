#ifndef FACTORY_H
#define FACTORY_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/BaseStatComponent.h"
#include "../Components/HPMPComponent.h"
#include "../Components/OffenseStatComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/ClassNameComponent.h"
#include "../Components/DynamicUIEntityComponent.h"
#include "../Components/DisplayStatComponent.h"
#include "../Components/AnimatedShootingComponent.h"
#include "../Components/CollisionFlagComponent.h"
#include "../Components/LootBagComponent.h"
#include "../Components/InteractUIComponent.h"
#include "../Components/ItemComponent.h"
#include "../Components/PlayerItemsComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimatedChaseAIComponent.h"
#include "../Components/AnimatedNeutralAIComponent.h"
#include "../Components/ChaseAIComponent.h"
#include "../Components/TrapAIComponent.h"
#include "../Components/ItemTableComponent.h"
#include "../Components/StatusEffectComponent.h"
#include "../Components/PortalComponent.h"
#include "../../libs/SDL2/SDL.h"
#include "../Utils/Xoshiro256.h"
#include "../Utils/tables.h"
#include "../Utils/CharacterManager.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/BossAIComponent.h"
#include "../Utils/room.h"
#include <unordered_set>
#include "../Utils/vechash.h"
#include "../Components/OscillatingProjectileComponent.h"
#include "../Components/LinearProjectileComponent.h"
#include "../Components/SecondaryProjectileComponent.h"
#include "../Components/isShootingComponent.h"
#include "../Components/DistanceToPlayerComponent.h"
#include "../Components/ParticleComponent.h"
#include "../Components/EnemySpawnerComponent.h"
#include "../Utils/BossIds.h"
#include "../Components/MinionSpawnerComponent.h"
#include "../Components/MinionComponent.h"
#include "../Components/OrbitalMovementComponent.h"
#include "../Components/OrbitShootMinionComponent.h"
#include "../../libs/glm/gtc/random.hpp"
#include "../Components/StandShootMinionComponent.h"
#include "../Components/OrbitMinionComponent.h"
#include "../Components/TowerComponent.h"
#include "../Components/InvisibleBossComponent.h"
#include "../Components/NeutralAIComponent.h"
#include "../Components/RandomChaseMinionComponent.h"
#include "../Components/DeathActionComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/PassiveAIComponent.h"
#include "../Components/HealOtherComponent.h"

/*
The factory class contains methods for spawning entities that represent important things such as 
lootbags, monsters, items, portals
*/

class Factory{
    private:
        Xoshiro256 RNG;
        std::vector<SDL_RendererFlip> flips = {SDL_FLIP_HORIZONTAL, SDL_FLIP_NONE};
        std::vector<float> particleAngles = std::vector<float>(12);

        inline float getRotationFromCoordiante(const float& projectileSpeed, const float& originX, const float& originY, const float& destX, const float& destY, glm::vec2& emitterVelocity, const bool& diagonal = false){
            float angleRadians = std::atan2(destY - originY, destX - originX);   
            float angleDegrees = angleRadians * (180.0 / M_PI);
            emitterVelocity.x = projectileSpeed * std::cos(angleRadians);
            emitterVelocity.y = projectileSpeed * std::sin(angleRadians);
            return fmod(angleDegrees + 90.0, 360.0) - 45*diagonal; // fmod shit because degrees=0 is top right
        }

    public:
    
        Factory();

        Entity spawnMonster(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum, int parentId = -1);

        Entity spawnGodLandsSpawner(std::unique_ptr<Registry>& registry, const room& room, int maxMonsters);

        void spawnAOEParticles(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, float radius, colors color);

        Entity creatLootBag(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum);

        void createItemInBag(std::unique_ptr<Registry>& registry, const items& itemEnum, Entity& lootbag);

        void createLootAtDeath(Entity& monster, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore);

        void createLootAtDeath(sprites spriteEnum, const glm::vec2& spawnPos, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore);

        void spawnPortal(std::unique_ptr<Registry>& registry, glm::vec2 spawnpoint, wallTheme area);

        void spawnVaultChests(std::unique_ptr<Registry>& registry, std::unique_ptr<CharacterManager>& CharacterManager);

        void populateDungeonWithMonsters(std::unique_ptr<Registry>& registry, std::vector<room>& dungeonRooms, wallTheme dungeonType, int bossRoomId, std::vector<BossIds>& bosses, std::map<cardinalDirection, room>& cardinalRooms);

        void spawnNumberEntity(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, std::string&& number);

        void spawnLinearParticle(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const glm::vec2 endpoint, float speed = 500);

        void spawnDecoration(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const sprites& spriteEnum = NONESPRITE);

        Entity spawnScepterParticles(Entity player, std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const glm::vec2& endpoint, int damage);

        void spawnHealOtherPartiles(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const glm::vec2& endpoint);

        void spawnScepterFailParticles(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, const glm::vec2& endpoint);

        void spawnNecromancerParticles(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, float radius);

        void spawnTreasureRoomChest(std::unique_ptr<Registry>& registry, const glm::vec2& spawnpoint, wallTheme area);

        void spawnAdminLootInNexus(std::unique_ptr<Registry>& registry);
};

#endif