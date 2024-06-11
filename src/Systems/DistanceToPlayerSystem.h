#ifndef DISTANCETOPLAYERSYSTEM_H
#define DISTANCETOPLAYERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/DistanceToPlayerComponent.h"
#include "../Components/BoxColliderComponent.h"

class DistanceToPlayerSystem: public System{
    private:
        inline float distance(const glm::vec2& origin, const glm::vec2& destination) {
            return sqrt(pow(origin.x - destination.x, 2) + pow(origin.y - destination.y, 2));
        }

    public:
        DistanceToPlayerSystem();
        void Update(const glm::vec2& playerPos);
};

#endif