#include "EnemySpawnSystem.h"

EnemySpawnSystem::EnemySpawnSystem(){
    RequireComponent<EnemySpawnerComponent>();
}

// this system continuously spawns monsters in the world. they are not minions. see minionSpawnSystem for that behavior
// currently only works for godlands, because it is the only place it is used
void EnemySpawnSystem::Update(Entity player, std::unique_ptr<Registry>& registry, std::unique_ptr<Factory>& factory, std::vector<BossIds>& bosses, const glm::vec2& playerPos){
    auto time = SDL_GetTicks();
    int chanceOfEventBoss = 50;
    const auto& playerHPMP = player.GetComponent<HPMPComponent>();
    auto playerHealthPercent = playerHPMP.activehp / playerHPMP.maxhp;
    for(auto& entity: GetSystemEntities()){
        auto& esc = entity.GetComponent<EnemySpawnerComponent>();
        // switch-case area can go here if this system is used in other areas in the future
        if(time >= 3000 + esc.lastSpawnTime){ // spawn monsters every X ms
            auto numGodLandsGods = registry->numEntitiesPerMonsterSubGroup(GODLANDSGOD);
            auto numEventBosses = registry->numEntitiesPerMonsterSubGroup(EVENTBOSS);
            const auto& room = esc.spawnRoom;
            esc.lastSpawnTime = time; // we will still wait a little until trying again regardless of whether monsters are spawned this frame
            if(numGodLandsGods + numEventBosses >= esc.maxMonsters){ // at monster capacity, exit function
                return; 
            }

            // if there are any gods, move 0-3 far ones closer to player
            int numToMove = RNG.randomFromRange(0,3);
            if(numGodLandsGods > 0 && numToMove > 0){
                constexpr float distanceToMove = 3000.0f;
                const auto& currentGods = registry->allEntitesFromMonsterSubGroup(GODLANDSGOD);
                int numMoved = 0;
                while(true){
                    for(int id: currentGods){ 
                        if(registry->GetComponent<DistanceToPlayerComponent>(id).distanceToPlayer > distanceToMove){
                            glm::vec2 spawnPos, spawnPosUnscaled;
                            do{
                                double distance = RNG.randomFromRange(1000.0,2000.0);
                                float randomAngle = glm::linearRand(0.0f, 6.2831855f);
                                float offsetX = distance * glm::cos(randomAngle); 
                                float offsetY = distance * glm::sin(randomAngle);
                                spawnPos = {(playerPos.x + offsetX), (playerPos.y + offsetY)};
                                spawnPosUnscaled = {(playerPos.x + offsetX)/64, (playerPos.y + offsetY)/64};
                            } while(!(spawnPosUnscaled.x > room.x + 4 && spawnPosUnscaled.x < room.x + room.w - 4 && spawnPosUnscaled.y > room.y + 4 && spawnPosUnscaled.y < room.y + room.h - 4));
                            registry->GetComponent<TransformComponent>(id).position = spawnPos;
                            numMoved++;
                            if(numMoved == numToMove){
                                break;
                            }
                        }
                    }
                    break;
                }
            }

            bool spawnedEventBossThisFrame = false;
            bool spawnedPotChestThisFrame = false;
            int numMonstersSpawnedThisFrame = 0;
            switch(numEventBosses){
                case 0:{
                    chanceOfEventBoss = 50; 
                } break;
                case 1:{
                    chanceOfEventBoss = 250;
                } break;
                case 2:{
                    chanceOfEventBoss = 500; 
                } break;
                case 3:{
                    chanceOfEventBoss = 1000;
                } break;
            }

            int chancePotChest = 50;
            if(playerHealthPercent <= .25){
                chancePotChest = 25;
            }

            while(numMonstersSpawnedThisFrame + numGodLandsGods + numEventBosses <= esc.maxMonsters){
                // spawn monster some distance away from player
                double distance = RNG.randomFromRange(1250.0,2250.0);
                double distanceMod;
                if(firstSpawn){ // only for the first spawning, spawn them further
                    distanceMod = RNG.randomFromRange(1.5,2.0);
                    distance *= distanceMod;
                }
                float randomAngle = glm::linearRand(0.0f, 6.2831855f);
                float offsetX = distance * glm::cos(randomAngle); 
                float offsetY = distance * glm::sin(randomAngle);
                glm::vec2 realSpawnPos = {(playerPos.x + offsetX), (playerPos.y + offsetY)};
                glm::vec2 spawnPosUnscaled = {(playerPos.x + offsetX)/64, (playerPos.y + offsetY)/64};
                // spawn monster if its not outside of the room 
                if(spawnPosUnscaled.x > room.x + 4 && spawnPosUnscaled.x < room.x + room.w - 4 && spawnPosUnscaled.y > room.y + 4 && spawnPosUnscaled.y < room.y + room.h - 4){
                    Entity spawnedEntity;
                    if(RNG.randomFromRange(1,chanceOfEventBoss) <= 1 && !firstSpawn && !spawnedEventBossThisFrame && numEventBosses < 3){ // spawn event god
                        sprites eventBoss = eventBosses[RNG.randomFromRange(0, eventBosses.size()-1)];
                        spawnedEntity = factory->spawnMonster(registry, realSpawnPos, eventBoss);
                        if(eventBoss != PENTARACT){
                            bosses.push_back({spawnedEntity.GetId(), spawnedEntity.GetCreationId()});    
                        }
                        spawnedEventBossThisFrame = true;
                    } else { // spawn regular god
                        if(!spawnedPotChestThisFrame && RNG.randomFromRange(1,chancePotChest) <= 1){ // 1/X chance to spawn a pot chest instead of a god
                            spawnedPotChestThisFrame = true;
                            spawnedEntity = factory->spawnMonster(registry, realSpawnPos, POTCHEST);
                        } else {
                            spawnedEntity = factory->spawnMonster(registry, realSpawnPos, gods[RNG.randomFromRange(0, gods.size()-1)]);   
                        }
                    }
                    numMonstersSpawnedThisFrame++;
                }
            }
            firstSpawn = false;
        }
    }   
}