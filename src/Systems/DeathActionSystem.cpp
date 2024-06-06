#include "DeathActionSystem.h"

DeathActionSystem::DeathActionSystem(){
    RequireComponent<DeathActionComponent>();
    RequireComponent<TransformComponent>();
}

void DeathActionSystem::Update(std::unique_ptr<Factory>& factory, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, std::vector<BossIds>& bosses){
    for(auto& entity: GetSystemEntities()){
        const auto& position = entity.GetComponent<TransformComponent>().position;
        if(registry->entityWasKilledThisFrame(entity)){
            const auto& spriteEnum = entity.GetComponent<DeathActionComponent>().spriteEnum;
            switch(spriteEnum){
                case PENTARACT:{
                    for(int i = 0; i < 5; ++i) { // find first open socket for the towerPosition
                        glm::vec2 pos = towerDestination(glm::vec2(position.x + 40.0f, position.y + 80.0f), -90.0f + (72.0f * i), 600.0f); 
                        factory->createLootAtDeath(spriteEnum, pos, registry, assetstore);
                    }
                } break;
                case MYSTERIOUSCRYSTAL:{
                    Entity boss = factory->spawnMonster(registry, position, CRYSTALPRISONER);
                    bosses.push_back({boss.GetId(), boss.GetCreationId()});  
                } break;
            }
        }
    }
}