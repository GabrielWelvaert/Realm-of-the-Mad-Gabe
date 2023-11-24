#ifndef PROJECTILEMOVEMENTSYSTEM_H
#define PROJECTILEMOVEMENTSYSTEM_H

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
        ProjectileMovementSystem();

        void Update(const double& deltaTime);

};

#endif