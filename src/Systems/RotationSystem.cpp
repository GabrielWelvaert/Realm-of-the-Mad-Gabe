#include "RotationSystem.h"


RotationSystem::RotationSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<RotationComponent>();
}

void RotationSystem::Update(const double& deltaTime){
    for(auto& entity: GetSystemEntities()){
        const auto& rc = entity.GetComponent<RotationComponent>();
        auto& rotation = entity.GetComponent<TransformComponent>().rotation;
        rotation += 2 * M_PI * rc.AngleChangePerDeltaTime * deltaTime;
    }
}
