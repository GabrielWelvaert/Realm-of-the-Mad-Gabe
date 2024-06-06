#ifndef MINIONSPAWNSYSTEM_H
#define MINIONSPAWNSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/MinionSpawnerComponent.h"
#include "../Utils/tables.h"
#include "../Utils/factory.h"
#include "../Utils/Xoshiro256.h"

class MinionSpawnSystem: public System{

    private:    
        Xoshiro256 RNG;
    public:
        MinionSpawnSystem();
        void Update(std::unique_ptr<Factory>& factory, std::unique_ptr<Registry>& registry, std::vector<BossIds>& bosses);

};


#endif