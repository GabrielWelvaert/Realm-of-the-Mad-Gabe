#ifndef RANDOMCHASEMINIONCOMPONENT_H
#define RANDOMCHASEMINIONCOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include "../../libs/glm/glm.hpp"

enum randomChaseStates{
    CHASE_PLAYER,
    CHASE_RANDOM_POINT_NEAR_PARENT,
};

struct RandomChaseMinionComponent{
    Uint32 timeOfLastSwitch = 0;
    Uint32 switchInterval;
    randomChaseStates state = CHASE_RANDOM_POINT_NEAR_PARENT;
    glm::vec2 destination;

    RandomChaseMinionComponent(Uint32 switchInterval):
        switchInterval(switchInterval) {}
        
    RandomChaseMinionComponent() = default;
};

#endif