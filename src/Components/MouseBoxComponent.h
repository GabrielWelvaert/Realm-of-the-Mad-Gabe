#pragma once 

#include "../../libs/SDL2/SDL.h"
#include "../../libs/glm/glm.hpp"
#include "../Utils/enums.h"
#include "../Utils/tables.h"

// used in the GUI for mouse-controlled things

struct MouseBoxComponent{
    unsigned long width;
    unsigned long height;
    glm::vec2 offset;

    MouseBoxComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0,0)){
        this->width = width;
        this->height = height;
        this->offset = offset;
    }

};