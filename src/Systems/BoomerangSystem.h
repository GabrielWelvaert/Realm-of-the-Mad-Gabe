#ifndef BOOMERANGSYSTEM_H
#define BOOMERANGSYSTEM_H

#include "../Components/BoomerangComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"
#include "../Utils/tables.h"

class BoomerangSystem: public System{
    public:
        BoomerangSystem();

        void Update();

};

#endif