#ifndef MINIONSPAWNSYSTEM_H
#define MINIONSPAWNSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/MinionSpawnerComponent.h"
#include "../Utils/tables.h"
#include "../Utils/factory.h"

class MinionSpawnSystem: public System{

    private:

    public:
        MinionSpawnSystem();
        void Update(std::unique_ptr<Factory>& factory, std::unique_ptr<Registry>& registry);

};


#endif