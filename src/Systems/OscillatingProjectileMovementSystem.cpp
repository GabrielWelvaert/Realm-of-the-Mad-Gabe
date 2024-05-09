#include "OscillatingProjectileMovementSystem.h"

OscillatingProjectileMovementSystem::OscillatingProjectileMovementSystem(){
    RequireComponent<ProjectileComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<OscillatingProjectileComponent>();
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
    // const SDL_Rect rect = {3,2,1,1};
    // const SDL_Rect rect2 = {3,5*8,1,1};
    for(auto& entity: GetSystemEntities()){
        auto& opc = entity.GetComponent<OscillatingProjectileComponent>();
        auto& realPosition = entity.GetComponent<TransformComponent>().position;
        auto& pc = entity.GetComponent<ProjectileComponent>();

        auto elapsedTime = time - pc.startTime;
        auto offsetY = std::sin(M_PI * opc.frequency * elapsedTime);
        auto offsetX = std::cos(M_PI * opc.frequency * elapsedTime);
        realPosition.y = opc.linearPosition.y + (offsetY * opc.amplitude);
        realPosition.x = opc.linearPosition.x + (offsetX * opc.amplitude);

        if(opc.inverse){ // this projectile is reflected about the origin
            realPosition.x = opc.linearPosition.x - (realPosition.x - opc.linearPosition.x);
            realPosition.y = opc.linearPosition.y - (realPosition.y - opc.linearPosition.y);
        }

        // Entity e = registry->CreateEntity();
        // e.AddComponent<TransformComponent>(realPosition);
        // e.AddComponent<SpriteComponent>(LOFIOBJ, 1, 1, rect, 15, false, false);

        // Entity l = registry->CreateEntity();
        // l.AddComponent<TransformComponent>(opc.linearPosition);
        // l.AddComponent<SpriteComponent>(LOFIOBJ, 1, 1, rect2, 15, false, false);
    }

}
