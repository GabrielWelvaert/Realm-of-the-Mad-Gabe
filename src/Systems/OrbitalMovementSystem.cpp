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
            const auto& speed = entity.GetComponent<SpeedStatComponent>().activespeed; // speed cant be 10
            auto& position = entity.GetComponent<TransformComponent>().position;
            double displacement = static_cast<double>(speed)/10.0 * deltaTime; // dividing by 10.0f creates floating imprecision. should 1 and -1 be 1.0 and 1.0f and -1.0f
            if(omc.inverse){
                displacement = -displacement;
            }
            omc.angle += displacement;
            position.x = omc.origin.x + omc.distance * std::cos(omc.angle);
            position.y = omc.origin.y + omc.distance * std::sin(omc.angle);
        }
    }
}