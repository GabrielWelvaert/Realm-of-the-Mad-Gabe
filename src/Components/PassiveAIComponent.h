#ifndef PASSIVEAICOMPONENT_H
#define PASSIVEAICOMPONENT_H

#include "../Utils/enums.h"
#include "../Utils/tables.h"
#include "../../libs/SDL2/SDL.h"
#include "../../libs/glm/glm.hpp"

struct PassiveAIComponent{
    Uint32 timeOfLastSwitch = 0;
    Uint32 switchInterval;
    glm::vec2 destination;

    inline PassiveAIComponent() = default;

    PassiveAIComponent(Uint32 switchInterval):
        switchInterval(switchInterval) {}
    
};

#endif