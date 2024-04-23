#include "ProjectileMovementSystem.h"

ProjectileMovementSystem::ProjectileMovementSystem(){
    RequireComponent<ProjectileComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<LinearProjectileComponent>();
}

void ProjectileMovementSystem::Update(const double& deltaTime, std::unique_ptr<Registry>& registry){
    const SDL_Rect rect2 = {3,5*8,1,1};
    for(auto& entity: GetSystemEntities()){
        auto& transform = entity.GetComponent<TransformComponent>();
        auto& rigidbody = entity.GetComponent<RidigBodyComponent>();
        transform.position.x += rigidbody.velocity.x * deltaTime;
        transform.position.y += rigidbody.velocity.y * deltaTime;

        // Entity l = registry->CreateEntity();
        // l.AddComponent<TransformComponent>(transform.position);
        // l.AddComponent<SpriteComponent>(LOFIOBJ, 1, 1, rect2, 15, false, false);
    }
}