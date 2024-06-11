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
    std::vector<sprites> minions;
    bool neverSpawnedMinions = true;
    bool spawnOnlyOnce;

    MinionSpawnerComponent() = default;

    MinionSpawnerComponent(std::vector<sprites> minions, int maxMinions, Uint32 respawnInterval, bool spawnOnlyOnce = false):
        respawnInterval(respawnInterval), maxMinions(maxMinions), minions(minions), spawnOnlyOnce(spawnOnlyOnce) {}
};

#endif