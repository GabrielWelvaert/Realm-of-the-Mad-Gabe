#include "DeathActionSystem.h"


DeathActionSystem::DeathActionSystem(){
    RequireComponent<DeathActionComponent>();
    RequireComponent<TransformComponent>();
}

void DeathActionSystem::Update(std::unique_ptr<Factory>& factory, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, std::vector<BossIds>& bosses){
    for(auto& entity: GetSystemEntities()){
        const auto& transform = entity.GetComponent<TransformComponent>();
        if(registry->entityWasKilledThisFrame(entity)){
            const auto& spriteEnum = entity.GetComponent<DeathActionComponent>().spriteEnum;
            switch(spriteEnum){
                case PENTARACT:{
                    for(int i = 0; i < 5; ++i) { // find first open socket for the towerPosition
                        glm::vec2 pos = towerDestination(glm::vec2(transform.position.x + 40.0f, transform.position.y + 80.0f), -90.0f + (72.0f * i), 600.0f); 
                        factory->createLootAtDeath(spriteEnum, pos, registry, assetstore);
                    }
                } break;
                case MYSTERIOUSCRYSTAL:{
                    Entity boss = factory->spawnMonster(registry, transform.position, CRYSTALPRISONER);
                    bosses.push_back({boss.GetId(), boss.GetCreationId()});  
                } break;
                case CHICKENEGG:{
                    factory->spawnMonster(registry, transform.position, RNG.randomFromVector(chickens));
                    factory->spawnDecoration(registry, transform.position, CRACKEDWHITEEGG);
                } break;
                case BROWNSLIMELARGE:{
                    const auto& sprite = entity.GetComponent<SpriteComponent>();
                    glm::vec2 center = {(transform.position.x + ((sprite.width * transform.scale.x) / 2)), transform.position.y + ((sprite.height * transform.scale.y) / 2)};
                    for(int i = 0; i < 2; i++){
                        glm::vec2 pos = towerDestination(center, -90.0f + ((360.0/2.0) * i), RNG.randomFromRange(0.0,15.0)) - 24.0f ;
                        factory->spawnMonster(registry, pos, BROWNSLIMEMEDIUM);
                    }
                } break;
                case BLACKSLIMELARGE:{
                    const auto& sprite = entity.GetComponent<SpriteComponent>();
                    glm::vec2 center = {(transform.position.x + ((sprite.width * transform.scale.x) / 2)), transform.position.y + ((sprite.height * transform.scale.y) / 2)};
                    for(int i = 0; i < 2; i++){
                        glm::vec2 pos = towerDestination(center, -90.0f + ((360.0/2.0) * i), RNG.randomFromRange(0.0,15.0)) - 24.0f ;
                        factory->spawnMonster(registry, pos, BLACKSLIMEMEDIUM);
                    }
                } break;
                case BROWNSLIMEMEDIUM:{
                    const auto& sprite = entity.GetComponent<SpriteComponent>();
                    glm::vec2 center = {(transform.position.x + ((sprite.width * transform.scale.x) / 2)), transform.position.y + ((sprite.height * transform.scale.y) / 2)};
                    for(int i = 0; i < 3; i++){
                        glm::vec2 pos = towerDestination(center, -90.0f + ((360.0/3.0) * i), RNG.randomFromRange(0.0,15.0)) - 16.0f;
                        factory->spawnMonster(registry, pos, BROWNSLIMESMALL); 
                    }
                } break;
                case BLACKSLIMEMEDIUM:{
                    const auto& sprite = entity.GetComponent<SpriteComponent>();
                    glm::vec2 center = {(transform.position.x + ((sprite.width * transform.scale.x) / 2)), transform.position.y + ((sprite.height * transform.scale.y) / 2)};
                    for(int i = 0; i < 3; i++){
                        glm::vec2 pos = towerDestination(center, -90.0f + ((360.0/3.0) * i), RNG.randomFromRange(0.0,15.0)) - 16.0f;
                        factory->spawnMonster(registry, pos, BLACKSLIMESMALL);
                    }
                } break;
            }
        }
    }
}