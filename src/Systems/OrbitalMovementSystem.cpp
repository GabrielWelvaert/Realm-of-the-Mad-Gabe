#include "OrbitalMovementSystem.h"

OrbitalMovementSystem::OrbitalMovementSystem(){
    RequireComponent<TransformComponent>();
    RequireComponent<RidigBodyComponent>();
    RequireComponent<SpeedStatComponent>();
    RequireComponent<OrbitalMovementComponent>();
    RequireComponent<SpriteComponent>();
}

//manually updates position. requires velocity to be at 0.0,0.0 so movementsystem "ignores" this entity
void OrbitalMovementSystem::Update(const double& deltaTime){
    for(auto& entity: GetSystemEntities()){
        auto& omc = entity.GetComponent<OrbitalMovementComponent>();
        if(omc.orbiting){
            const auto& speed = entity.GetComponent<SpeedStatComponent>().activespeed;
            auto& position = entity.GetComponent<TransformComponent>().position;
            omc.angle += (omc.inverse ? -1 : 1) * static_cast<float>(speed/10) * deltaTime;
            position.x = omc.origin.x + omc.distance * std::cos(omc.angle);
            position.y = omc.origin.y + omc.distance * std::sin(omc.angle);
        }
    }
}