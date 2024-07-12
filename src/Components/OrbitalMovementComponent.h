#ifndef ORBITALMOVEMENTCOMPONENT_H
#define ORBITALMOVEMENTCOMPONENT_H

#include "../Utils/tables.h"
#include "../../libs/glm/glm.hpp"

struct OrbitalMovementComponent{
    float distance; // (closest) distance from parent, used for whatever calculation
    glm::vec2 origin;
    bool orbiting = false;
    bool inverse;
    float angle = 0.0f; // angle (radians) used for orbital movement
    OrbitalMovementComponent() = default;
    OrbitalMovementComponent(float distance, bool inverse, glm::vec2 origin = {0.0,0.0}):
        distance(distance), origin(origin), inverse(inverse) {}
    OrbitalMovementComponent(float distance, bool inverse, float angle):
        distance(distance), angle(angle), inverse(inverse) {}
    
};

#endif