#include "OscillatingProjectileMovementSystem.h"

OscillatingProjectileMovementSystem::OscillatingProjectileMovementSystem(){
    RequireComponent<ProjectileComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<OscillatingProjectileComponent>();
    RequireComponent<SpriteComponent>();
}

void OscillatingProjectileMovementSystem::UpdateSimulatedPositions(const double& deltaTime){
    for(auto& entity: GetSystemEntities()){
        auto& velocity = entity.GetComponent<RidigBodyComponent>().velocity;
        auto& linearPosition = entity.GetComponent<OscillatingProjectileComponent>().linearPosition;
        linearPosition.x += velocity.x * deltaTime;
        linearPosition.y += velocity.y * deltaTime;
    }   
}

void OscillatingProjectileMovementSystem::UpdateRealPositions(std::unique_ptr<Registry>& registry){
    auto time = SDL_GetTicks();
    for(auto& entity: GetSystemEntities()){
        auto& opc = entity.GetComponent<OscillatingProjectileComponent>();
        auto& transform = entity.GetComponent<TransformComponent>();
        auto& pc = entity.GetComponent<ProjectileComponent>();
        auto elapsedTime = time - pc.startTime;

        // note on staff projectiles:
        // I tried for hours to get it to be perfectly symmetrical for the regular-inverterd pair staff projectiles
        // mathematically, the position inversion was perfect (inverted displacement is negative regular displacement)
        // on screen, it is never perfectly symmetrical. I assumed this was becuase of how position is origin of sprite 
        // but even when accounting for that, it never looked right. even 1x1 pixel projectiles looked asymmetrical
        // modifying the position with respect to sprite dimensions and origins didnt change the fact that the "curve" was off! 

        auto offsetY = std::sin(M_PI * opc.frequency * elapsedTime);
        auto offsetX = std::cos(M_PI * opc.frequency * elapsedTime);
        if(!opc.inverse){ 
            transform.position.y = opc.linearPosition.y + (offsetY * opc.amplitude);
            transform.position.x = opc.linearPosition.x + (offsetX * opc.amplitude);
        } else { 
            transform.position.y = opc.linearPosition.y - (offsetY * opc.amplitude);
            transform.position.x = opc.linearPosition.x - (offsetX * opc.amplitude);
        }
    }
}
