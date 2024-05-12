#ifndef ENEMYSPAWNSYSTEM_H
#define ENEMYSPAWNSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/EnemySpawnerComponent.h"
#include "../../libs/SDL2/SDL.h"
#include "../Utils/Xoshiro256.h"
#include "../../libs/glm/glm.hpp"
#include "../../libs/glm/gtc/random.hpp"
#include "../Components/TransformComponent.h"
#include "../Utils/tables.h"
#include "../Utils/factory.h"

class EnemySpawnSystem: public System{

    private:
        unsigned int numEnemiesSpawned = 0;
        Xoshiro256 RNG;

    public:
        EnemySpawnSystem();
        void Update(Entity player, int numMonsters, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory);

};



#endif