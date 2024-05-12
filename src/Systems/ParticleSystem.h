#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/ParticleComponent.h"

class ParticleSystem: public System{
    public:
        ParticleSystem();

        void Update(const double& deltaTime);
};

#endif