#pragma once 

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"
#include "../../libs/SDL2/SDL.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"

// Projectiles have their own movement system because, unlike players/monsters,
// they lack the need for collisionFlags for wall-collision resolution
// I believe this serparation of movement systems 
// has greatly increased cache-hits for projectile movement

class ProjectileMovementSystem: public System{
    public:
        ProjectileMovementSystem(){
            RequireComponent<ProjectileComponent>();
            RequireComponent<RidigBodyComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(const double& deltaTime){
            for(auto entity: GetSystemEntities()){
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rigidbody = entity.GetComponent<RidigBodyComponent>();
                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;
            }
        }

};