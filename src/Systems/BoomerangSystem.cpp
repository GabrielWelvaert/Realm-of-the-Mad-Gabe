#include "BoomerangSystem.h"

BoomerangSystem::BoomerangSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<ProjectileComponent>();
    RequireComponent<BoomerangComponent>();
}

void BoomerangSystem::Update(){
    auto time = SDL_GetTicks();
    for(auto& entity: GetSystemEntities()){
        auto& bc = entity.GetComponent<BoomerangComponent>();
        if(!bc.invertedVelocity){
            const auto& pc = entity.GetComponent<ProjectileComponent>();    
            if(time >= pc.startTime + bc.halfduration){
                auto& velocity = entity.GetComponent<RidigBodyComponent>().velocity;
                velocity *= -1.0f;
                bc.invertedVelocity = true;
            }
        }
    }
}