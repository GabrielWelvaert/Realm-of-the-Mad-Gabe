#include "DeathActionSystem.h"


DeathActionSystem::DeathActionSystem(){
    RequireComponent<DeathActionComponent>();
    RequireComponent<TransformComponent>();
}

void DeathActionSystem::Update(std::unique_ptr<Factory>& factory, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetstore, std::vector<BossIds>& bosses){
    for(auto& entity: GetSystemEntities()){
        auto * transform = &entity.GetComponent<TransformComponent>();
        if(registry->entityWasKilledThisFrame(entity)){
            auto * spriteEnum = &entity.GetComponent<DeathActionComponent>().spriteEnum;
            switch(*spriteEnum){
                case DRAGONEGG:{
                    factory->spawnMonster(registry, transform->position, SMALLDRAGON);
                    spriteEnum = &entity.GetComponent<DeathActionComponent>().spriteEnum;
                    transform = &entity.GetComponent<TransformComponent>();
                    factory->spawnDecoration(registry, transform->position - 4.0f, CRACKEDDRAGONEGG);
                } break;
                case SMALLDRAGON:{
                    factory->spawnMonster(registry, transform->position, MEDIUMDRAGON);
                } break;
                case MEDIUMDRAGON:{
                    factory->spawnMonster(registry, transform->position, LARGEDRAGON);
                } break;
                case PENTARACT:{ // spawn loot and kill all eyes
                    for(int i = 0; i < 5; ++i) { // find first open socket for the towerPosition
                        glm::vec2 pos = towerDestination(glm::vec2(transform->position.x + 40.0f, transform->position.y + 80.0f), -90.0f + (72.0f * i), 600.0f); 
                        factory->createLootAtDeath(*spriteEnum, pos, registry, assetstore);
                    }
                } break;
                case MYSTERIOUSCRYSTAL:{
                    Entity boss = factory->spawnMonster(registry, transform->position, CRYSTALPRISONER);
                    bosses.push_back({boss.GetId(), boss.GetCreationId()});  
                } break;
                case CHICKENEGG:{
                    factory->spawnMonster(registry, transform->position, RNG.randomFromVector(chickens));
                    spriteEnum = &entity.GetComponent<DeathActionComponent>().spriteEnum;
                    transform = &entity.GetComponent<TransformComponent>();
                    factory->spawnDecoration(registry, transform->position - 4.0f, CRACKEDWHITEEGG);
                } break;
                case BROWNSLIMELARGE:{
                    const auto& sprite = entity.GetComponent<SpriteComponent>();
                    glm::vec2 center = {(transform->position.x + ((sprite.width * transform->scale.x) / 2)), transform->position.y + ((sprite.height * transform->scale.y) / 2)};
                    for(int i = 0; i < 2; i++){
                        glm::vec2 pos = towerDestination(center, -90.0f + ((360.0/2.0) * i), RNG.randomFromRange(0.0,15.0)) - 24.0f ;
                        factory->spawnMonster(registry, pos, BROWNSLIMEMEDIUM);
                    }
                } break;
                case BLACKSLIMELARGE:{
                    const auto& sprite = entity.GetComponent<SpriteComponent>();
                    glm::vec2 center = {(transform->position.x + ((sprite.width * transform->scale.x) / 2)), transform->position.y + ((sprite.height * transform->scale.y) / 2)};
                    for(int i = 0; i < 2; i++){
                        glm::vec2 pos = towerDestination(center, -90.0f + ((360.0/2.0) * i), RNG.randomFromRange(0.0,15.0)) - 24.0f ;
                        factory->spawnMonster(registry, pos, BLACKSLIMEMEDIUM);
                    }
                } break;
                case BROWNSLIMEMEDIUM:{
                    const auto& sprite = entity.GetComponent<SpriteComponent>();
                    glm::vec2 center = {(transform->position.x + ((sprite.width * transform->scale.x) / 2)), transform->position.y + ((sprite.height * transform->scale.y) / 2)};
                    for(int i = 0; i < 3; i++){
                        glm::vec2 pos = towerDestination(center, -90.0f + ((360.0/3.0) * i), RNG.randomFromRange(0.0,15.0)) - 16.0f;
                        factory->spawnMonster(registry, pos, BROWNSLIMESMALL); 
                    }
                } break;
                case BLACKSLIMEMEDIUM:{
                    const auto& sprite = entity.GetComponent<SpriteComponent>();
                    glm::vec2 center = {(transform->position.x + ((sprite.width * transform->scale.x) / 2)), transform->position.y + ((sprite.height * transform->scale.y) / 2)};
                    for(int i = 0; i < 3; i++){
                        glm::vec2 pos = towerDestination(center, -90.0f + ((360.0/3.0) * i), RNG.randomFromRange(0.0,15.0)) - 16.0f;
                        factory->spawnMonster(registry, pos, BLACKSLIMESMALL);
                    }
                } break;
                case SIMBA:{
                    const auto& center = entity.GetComponent<TransformComponent>().center;
                    factory->spawnMonster(registry, center, SIMBA);
                } break;
                case MALPHAS:{
                    auto * center = &entity.GetComponent<TransformComponent>().center;
                    factory->spawnMonster(registry, *center + 20.0f, MALPHASBABY);
                    spriteEnum = &entity.GetComponent<DeathActionComponent>().spriteEnum;
                    center = &entity.GetComponent<TransformComponent>().center;
                    factory->spawnMonster(registry, *center - 20.0f, MALPHASBABY);
                } break;
                case ABYSSTOWER:{
                    const auto& t = entity.GetComponent<TransformComponent>();
                    const auto& s = entity.GetComponent<SpriteComponent>();
                    glm::vec2 spawnpoint = {(t.position.x + (s.width * t.scale.x / 2) - 20), (t.position.y + (s.height * t.scale.y / 2) - 20) + 64.0f};
                    factory->spawnPortal(registry, spawnpoint, SPAWNTELEPORT);
                } break;
                case GORDON:{
                    const auto& aidata = entity.GetComponent<BossAIComponent>();
                    factory->spawnPortal(registry, aidata.spawnPoint + 32.0f, GORDONSLAIR2); // todo spawn portal to gordon 2
                    for(int i = 0; i < 3; i++){
                        double distance = RNG.randomFromRange(50.0,300.0);
                        float randomAngle = glm::linearRand(0.0f, 6.2831855f);
                        float offsetX = distance * glm::cos(randomAngle); 
                        float offsetY = distance * glm::sin(randomAngle);
                        glm::vec2 spawnPos = {(aidata.spawnPoint.x + offsetX), (aidata.spawnPoint.y + offsetY)};
                        factory->spawnMonster(registry, spawnPos, POTCHEST);
                    }
                } break;
                case SPRITEGOD: 
                case MEDUSA: 
                case DJINN:
                case ENTGOD:
                case BEHOLDER:
                case FLYINGBRAIN:
                case SLIMEGOD:
                case GHOSTGOD:
                case WHITEDEMON:
                case LEVIATHAN:{ // godlands bosses have a chance to spawn a portal to the abyss
                    if(RNG.randomFromRange(1,200) == 1){
                        const auto& t = entity.GetComponent<TransformComponent>();
                        const auto& s = entity.GetComponent<SpriteComponent>();
                        glm::vec2 spawnpoint = {(t.position.x + (s.width * t.scale.x / 2) - 20), (t.position.y + (s.height * t.scale.y / 2) - 20) + 64.0f};
                        factory->spawnPortal(registry, spawnpoint, ABYSS);
                    }
                } break;
            }
        }
    }
}