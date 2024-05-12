#include "EnemySpawnSystem.h"

EnemySpawnSystem::EnemySpawnSystem(){
    RequireComponent<EnemySpawnerComponent>();
}

// pretty much only works for godlands, because its not used anywhere else right now
void EnemySpawnSystem::Update(Entity player, int numMonsters, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory){
    auto time = SDL_GetTicks();
    for(auto& entity: GetSystemEntities()){
        auto& esc = entity.GetComponent<EnemySpawnerComponent>();
        if(time + 10000 >= esc.lastSpawnTime){ // spawn monsters every 10 seconds
            esc.lastSpawnTime = time;
            int numMonstersSpawnedThisFrame = 0;
            const auto& playerPos = player.GetComponent<TransformComponent>().position;
            while(numMonstersSpawnedThisFrame + numMonsters <= esc.maxMonsters){
                // spawn monster some distance away from player
                const auto& room = esc.spawnRoom;
                int distance = RNG.randomFromRange(1000,2000);
                if(numMonsters - 1 == 0){ // first frame in-dungeon, spawn a ton but further away
                    distance *= RNG.randomFromRange(1.5,2.0);
                }
                float randomAngle = glm::linearRand(0.0f, 6.2831855f);
                float offsetX = distance * glm::cos(randomAngle);
                float offsetY = distance * glm::sin(randomAngle);
                glm::vec2 realSpawnPos = {(playerPos.x + offsetX), (playerPos.y + offsetY)};
                glm::vec2 spawnPosUnscaled = {(playerPos.x + offsetX)/64, (playerPos.y + offsetY)/64};
                // spawn monster if its not outside of the room 
                if(spawnPosUnscaled.x > room.x + 4 && spawnPosUnscaled.x < room.x + room.w - 4 && spawnPosUnscaled.y > room.y + 4 && spawnPosUnscaled.y < room.y + room.h - 4){
                    factory->spawnMonster(registry, realSpawnPos, gods[RNG.randomFromRange(0, gods.size()-1)]);
                    numMonstersSpawnedThisFrame++;
                } else {
                    std::cout << "illegal god spawn blocked" << '\n';
                }
            }
        }
        
    }   
}