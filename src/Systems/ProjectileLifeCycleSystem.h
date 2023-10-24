#ifndef PROJECTILELIFECYCLESYSTEM_H
#define PROJECTILELIFECYCLESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"
#include "../../libs/SDL2/SDL.h"

/*
this system is responsible for killing projectiles if they have expired
*/

class ProjectileLifeCycleSystem: public System{
public:
    ProjectileLifeCycleSystem();
    void Update();
};

#endif