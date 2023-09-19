#ifndef BOXCOLLIDERCOMPONENT_H 
#define BOXCOLLIDERCOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include "../../libs/glm/glm.hpp"
#include "../Utils/enums.h"
#include "../Utils/tables.h"

struct BoxColliderComponent {
    unsigned long width;
    unsigned long height;
    glm::vec2 offset;

    BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0,0)){
        this->width = width;
        this->height = height;
        this->offset = offset;
    }

    //constructor for standard sized player and enemies:
    BoxColliderComponent(boxColliders type){
        boxColliderData data = bcEnumToData.at(type);
        this->width = data.width;
        this->height = data.height;
        this->offset = data.offset;
    }

    BoxColliderComponent(sprites spriteEnum){
        boxColliders bc = spriteToBC.at(spriteEnum);
        boxColliderData data = bcEnumToData.at(bc);
        this->width = data.width;
        this->height = data.height;
        this->offset = data.offset;
    }

};

#endif