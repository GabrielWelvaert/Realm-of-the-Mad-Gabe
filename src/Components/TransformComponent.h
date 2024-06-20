#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "../../libs/glm/glm.hpp"
#include "../Utils/enums.h"

struct TransformComponent {
    glm::vec2 position;
    glm::vec2 scale;
    double rotation;    
    glm::vec2 center; // optional member field which holds center relative to a spritecomponent. if no true center, it holds regular position

    inline TransformComponent(glm::vec2 position = glm::vec2(0,0), glm::vec2 scale = glm::vec2(6.0,6.0), double rotation = 0.0):
        position(position),
        scale(scale),
        rotation(rotation),
        center(position)
        {}

};

#endif