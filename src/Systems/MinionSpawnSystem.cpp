#include "MinionSpawnSystem.h"

MinionSpawnSystem::MinionSpawnSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<MinionSpawnerComponent>();
}

void MinionSpawnSystem::Update(std::unique_ptr<Factory>& factory, std::unique_ptr<Registry>& registry, std::vector<BossIds>& bosses){
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
            // this loop was previously a range-based for loop, but this invalidates iterators when we .erase and causes undefined behavior
            // https://stackoverflow.com/a/29571569/20080198 
            // removing dead minions from minionIdToCreationId
            for(auto it = msc.minionIdToCreationId.begin(); it != msc.minionIdToCreationId.end();){
                if(!registry->entityIsAlive(it->first, it->second)){
                    it = msc.minionIdToCreationId.erase(it); 
                }else{
                    ++it; 
                }
            }
            if(msc.minionIdToCreationId.size() == msc.maxMinions){
                continue;
            }
            
            // spawn new minions to replace dead ones
            const auto& pt = entity.GetComponent<TransformComponent>();
            glm::vec2 center;
            if(entity.HasComponent<SpriteComponent>()){
                const auto& ps = entity.GetComponent<SpriteComponent>();
                center = {pt.position.x + ((ps.height * pt.scale.x)/2) - 24, pt.position.y + ((ps.width * pt.scale.y)/2) - 24};
            } else {
                center = pt.position;
            }
            for(int i = msc.minionIdToCreationId.size(); i < msc.maxMinions; i++){
                sprites s = RNG.randomFromVector(msc.minions);
                Entity minion = factory->spawnMonster(registry, center, s, entity.GetId());
                msc.minionIdToCreationId[minion.GetId()] = registry->GetCreationIdFromEntityId(minion.GetId());
                switch(s){ // should this mosnter be considered as a boss?
                    case PENTARACTTOWER:{
                        bosses.push_back({minion.GetId(), minion.GetCreationId()});
                    } break;
                }
            }
        }
    }

}