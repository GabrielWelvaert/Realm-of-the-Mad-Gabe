#ifndef MOUSEBOXCOMPONENT_H
#define MOUSEBOXCOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include "../../libs/glm/glm.hpp"
#include "../Utils/enums.h"
#include "../Utils/tables.h"

// used in the GUI for mouse-controlled things

struct MouseBoxComponent{
    unsigned long width;
    unsigned long height;
    glm::vec2 offset;

    inline MouseBoxComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0,0)):
        width(width),
        height(height),
        offset(offset)
        {}

};

#endif