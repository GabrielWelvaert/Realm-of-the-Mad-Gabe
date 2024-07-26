#include "ParabolicMovementSystem.h"

ParabolicMovementSystem::ParabolicMovementSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<ParabolicMovementComponent>();
}

void ParabolicMovementSystem::Update(Entity player, const double& deltaTime, 
                        std::unique_ptr<EventBus>& eventBus,
                        std::unique_ptr<Registry>& registry, 
                        std::unique_ptr<AssetStore>& assetStore, 
                        std::unique_ptr<Factory>& factory, 
                        std::function<void(bool, bool, wallTheme)> Setup, 
                        DeadPlayer& deadPlayer, 
                        std::string& activeCharacterID, 
                        std::unique_ptr<CharacterManager>& characterManager){

    for(auto& entity: GetSystemEntities()){
        auto * position = &entity.GetComponent<TransformComponent>().position;
        auto& pmc = entity.GetComponent<ParabolicMovementComponent>();

        // adapted from https://gamedev.stackexchange.com/a/207373/179377
        if(pmc.fireLerp < pmc.finalLerp){ // parabolic entity has not completed its path of travel
            glm::vec3 linearProgress = glm::mix(pmc.startPos, pmc.endPos, pmc.fireLerp); // linear interpolation
            float perspectiveOffset = std::sin(pmc.fireLerp * M_PI) * pmc.height;
            glm::vec3 new3dPosition = linearProgress - (glm::vec3(0.0f, 1.0f, 0.0f) * perspectiveOffset); // 3d inernal representation 
            *position = {new3dPosition.x, new3dPosition.y + new3dPosition.z}; // 2d representation, to be rendered
            pmc.fireLerp += pmc.speed * deltaTime; // record progress of completing path of travel in fireLerp
        
        } else { // parabolic entity has completed its path of travel
            switch(pmc.identifier){ // what happens when this entity finishes its path of travel
                case PARABOLIC_MEDUSA_AOE_BOMB:{ // ex, medusa bomb, hitboxes exist for 1 frame only 
                    eventBus->EmitEvent<AOEBombEvent>(pmc.sprite, player, entity, *position, 192.0, eventBus, registry, assetStore, factory, Setup, deadPlayer, activeCharacterID, characterManager);
                    position = &entity.GetComponent<TransformComponent>().position;
                    factory->spawnAOEParticles(registry, *position, 192.0, RED);
                } break;
                case PARABOLIC_BLACK_AOE_BOMB:{
                    eventBus->EmitEvent<AOEBombEvent>(pmc.sprite, player, entity, *position, 192.0, eventBus, registry, assetStore, factory, Setup, deadPlayer, activeCharacterID, characterManager);
                    position = &entity.GetComponent<TransformComponent>().position;
                    factory->spawnAOEParticles(registry, *position, 192.0, BLACK);
                } break;
                case GORDON_SLOW_BOMB:{
                    eventBus->EmitEvent<AOEBombEvent>(pmc.sprite, player, entity, *position, 1000.0, eventBus, registry, assetStore, factory, Setup, deadPlayer, activeCharacterID, characterManager);
                    position = &entity.GetComponent<TransformComponent>().position;
                    factory->spawnAOEParticles(registry, *position, 10000.0, BLACK);
                } break;
                case PARABOLIC_TRAP:{ // ex huntress trap, spawn lingering hitbox. not implemented, not used
                } break;
                case PARABOLIC_BULLET:{ // don't do anything, entity was a real bullet (would be really weird) ! not implemented, not used
                } break;
            }
            entity.Kill();
        }
    }
}