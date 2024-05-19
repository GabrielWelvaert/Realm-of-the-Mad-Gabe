#include "MinionSpawnSystem.h"

MinionSpawnSystem::MinionSpawnSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<MinionSpawnerComponent>();
    RequireComponent<SpriteComponent>();
}

void MinionSpawnSystem::Update(std::unique_ptr<Factory>& factory, std::unique_ptr<Registry>& registry){
    auto time = SDL_GetTicks();
    for(auto& entity: GetSystemEntities()){ // monster that spawns minions
        auto& msc = entity.GetComponent<MinionSpawnerComponent>();
        if(time > msc.timeOfLastRespawn + msc.respawnInterval){ // spawn minions every msc.respawnInterval MS

            // if this entity is a minion itself, and its parent has died, do not spawn more minions
            if(entity.HasComponent<MinionComponent>()){
                const auto& mc = entity.GetComponent<MinionComponent>();
                if(!registry->entityIsAlive(mc.idOfParent, mc.creationIdOfParent)){
                    continue;
                }
            }

            msc.timeOfLastRespawn = time;
            for(auto& pair: msc.minionIdToCreationId){ // update map to remove dead minions
                if(!registry->entityIsAlive(pair.first, pair.second)){
                    msc.minionIdToCreationId.erase(pair.first);
                }
            }
            if(msc.minionIdToCreationId.size() == msc.maxMinions){
                continue;
            }
            
            // spawn new minions to replace dead ones
            const auto& pt = entity.GetComponent<TransformComponent>();
            const auto& ps = entity.GetComponent<SpriteComponent>();
            glm::vec2 center = {pt.position.x + ((ps.height * pt.scale.x)/2) - 24, pt.position.y + ((ps.width * pt.scale.y)/2) - 24};
            for(int i = msc.minionIdToCreationId.size(); i < msc.maxMinions; i++){
                Entity minion = factory->spawnMonster(registry, center, msc.monsterToSpawn, entity.GetId());
                msc.minionIdToCreationId[minion.GetId()] = registry->GetCreationIdFromEntityId(minion.GetId());
            }
        }
    }

}