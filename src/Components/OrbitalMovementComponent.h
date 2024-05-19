#ifndef ORBITALMOVEMENTCOMPONENT_H
#define ORBITALMOVEMENTCOMPONENT_H

#include "../Utils/tables.h"
#include "../../libs/glm/glm.hpp"
#include "../../libs/glm/gtc/random.hpp"

struct OrbitalMovementComponent{
    float distance; // (closest) distance from parent, used for whatever calculation
    glm::vec2 origin;
    glm::vec2 escapeVelocity; // angle used to path to orbital radius (ex spawn on boss, take this path)
    bool orbiting = false;
    bool inverse;
    float angle = 0.0f; // angle used for orbital movement
    OrbitalMovementComponent() = default;
    OrbitalMovementComponent(float distance, bool inverse, glm::vec2 origin = {0.0,0.0}):
        distance(distance), origin(origin), inverse(inverse) {
            float randomAngle = glm::linearRand(0.0f, 6.2831855f);
            escapeVelocity = {glm::cos(randomAngle), glm::sin(randomAngle)};
        }
    
};

#endif