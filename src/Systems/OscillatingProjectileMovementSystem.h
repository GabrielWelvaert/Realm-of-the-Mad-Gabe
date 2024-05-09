#ifndef OSCILLATINGPROJECTILEMOVEMENTSYSTEM_H
#define OSCILLATINGPROJECTILEMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/OscillatingProjectileComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../../libs/SDL2/SDL.h"
#include <cmath>
#include "../Components/SpriteComponent.h" 

class OscillatingProjectileMovementSystem: public System{
    public:
        OscillatingProjectileMovementSystem();

        // update simulated linear position of projectile
        void UpdateSimulatedPositions(const double& deltaTime);

        // update oscillation position (real position) of projectile
        void UpdateRealPositions(std::unique_ptr<Registry>& registry);


};

#endif