#ifndef ENEMYSPAWNERCOMPONENT_H
#define ENEMYSPAWNERCOMPONENT_H

#include "../Utils/room.h"
#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"

struct EnemySpawnerComponent{   
    room spawnRoom;
    int maxMonsters;
    Uint32 lastSpawnTime = 0;
    inline EnemySpawnerComponent() = default;
    inline EnemySpawnerComponent(room spawnRoom, int maxMonsters): spawnRoom(spawnRoom), maxMonsters(maxMonsters) {}
};



#endif