#include "ProjectileLifeCycleSystem.h"

ProjectileLifeCycleSystem::ProjectileLifeCycleSystem(){
    RequireComponent<ProjectileComponent>();
}

void ProjectileLifeCycleSystem::Update(){
    for(auto entity: GetSystemEntities()){
        auto projectile = entity.GetComponent<ProjectileComponent>();
        if(SDL_GetTicks() - projectile.startTime > projectile.duration){
            entity.Kill();
        }
    }
}