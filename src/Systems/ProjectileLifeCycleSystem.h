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
    ProjectileLifeCycleSystem(){
        RequireComponent<ProjectileComponent>();
    }

    void Update(){
        for(auto entity: GetSystemEntities()){
            auto projectile = entity.GetComponent<ProjectileComponent>();
            //kill projectile if they have existed for >= their duration time!
            if(SDL_GetTicks() - projectile.startTime > projectile.duration){
                entity.Kill();

            }
        }
    }
};

#endif