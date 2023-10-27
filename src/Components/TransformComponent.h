#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "../../libs/glm/glm.hpp"
#include "../Utils/enums.h"

struct TransformComponent {
    glm::vec2 position;
    glm::vec2 scale;
    double rotation;    

    inline TransformComponent(glm::vec2 position = glm::vec2(0,0), glm::vec2 scale = glm::vec2(6.0,6.0), double rotation = 0.0):
        position(position),
        scale(scale),
        rotation(rotation)
        {}

};

#endif