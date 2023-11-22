#include "ProjectileMovementSystem.h"

ProjectileMovementSystem::ProjectileMovementSystem(){
    RequireComponent<ProjectileComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<TransformComponent>();
}

void ProjectileMovementSystem::Update(const double& deltaTime){
    for(auto& entity: GetSystemEntities()){
        auto& transform = entity.GetComponent<TransformComponent>();
        auto& rigidbody = entity.GetComponent<RidigBodyComponent>();
        transform.position.x += rigidbody.velocity.x * deltaTime;
        transform.position.y += rigidbody.velocity.y * deltaTime;
    }
}