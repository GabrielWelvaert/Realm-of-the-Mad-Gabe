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
        auto& transform = entity.GetComponent<TransformComponent>();
        auto& velocity = entity.GetComponent<RidigBodyComponent>().velocity;
        auto& pc = entity.GetComponent<ParticleComponent>();
        constexpr int shrinkinternval = 200;
        constexpr float minimumParticleScale = 3.0f;
        constexpr float shrinkFactor = 1.0f/1.5f;

        if(pc.shrinking && time >= pc.timeOflastShrink + shrinkinternval){ // time to shrink
            const auto& sprite = entity.GetComponent<SpriteComponent>();
            pc.timeOflastShrink = time; 
            auto oldsize = sprite.width * transform.scale.x;
            transform.scale *= shrinkFactor;
            transform.position += (oldsize - (sprite.width * transform.scale.x))/2.0f;

        } 
        transform.position.x += velocity.x * deltaTime;
        transform.position.y += velocity.y * deltaTime;
        if(time >= pc.deathTime || transform.scale.x <= minimumParticleScale){
            entity.Kill();
        }
    }
}