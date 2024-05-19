#ifndef ORBITALMOVEMENTSYSTEM_H
#define ORBITALMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../../libs/glm/glm.hpp"
#include "../Components/TransformComponent.h"
#include "../Components/OrbitalMovementComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpeedStatComponent.h"
#include "../Components/SpriteComponent.h"

class OrbitalMovementSystem: public System{
    public:
        OrbitalMovementSystem();
        void Update(const double& deltaTime);

};


#endif