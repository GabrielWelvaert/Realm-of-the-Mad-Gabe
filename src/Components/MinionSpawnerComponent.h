#ifndef MINIONSPAWNERCOMPONENT_H
#define MINIONSPAWNERCOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include <vector>
#include "../Utils/tables.h"

struct MinionSpawnerComponent{
    Uint32 respawnInterval;
    Uint32 timeOfLastRespawn = 0;
    unsigned char maxMinions;
    std::unordered_map<int,int> minionIdToCreationId;
    sprites monsterToSpawn;

    MinionSpawnerComponent() = default;

    MinionSpawnerComponent(sprites monsterToSpawn, int maxMinions, Uint32 respawnInterval = 10000):
        respawnInterval(respawnInterval), maxMinions(maxMinions), monsterToSpawn(monsterToSpawn) {}
};

#endif