#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<SpriteComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<ParticleComponent>();
}

void ParticleSystem::Update(const double& deltaTime){
    auto time = SDL_GetTicks();
    for(auto& entity: GetSystemEntities()){
        auto& position = entity.GetComponent<TransformComponent>().position;
        auto& velocity = entity.GetComponent<RidigBodyComponent>().velocity;
        const auto& deathTime = entity.GetComponent<ParticleComponent>().deathTime;

        if(time >= deathTime){
            entity.Kill();
        } else {
            position.x += velocity.x * deltaTime;
            position.y += velocity.y * deltaTime;
        }
        
    }
}