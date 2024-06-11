#include "DistanceToPlayerSystem.h"

DistanceToPlayerSystem::DistanceToPlayerSystem(){
    RequireComponent<DistanceToPlayerComponent>();
    RequireComponent<TransformComponent>();
    RequireComponent<BoxColliderComponent>();
}

void DistanceToPlayerSystem::Update(const glm::vec2& playerPos){
    for(auto& entity: GetSystemEntities()){
        const auto& monsterPos = entity.GetComponent<TransformComponent>().position;  
        auto& distanceToPlayer = entity.GetComponent<DistanceToPlayerComponent>().distanceToPlayer;
        distanceToPlayer = distance(monsterPos, playerPos);
    }
}