#ifndef PARABOLICMOVEMENTCOMPONENT_H
#define PARABOLICMOVEMENTCOMPONENT_H

#include "../../libs/SDL2/SDL.h"

struct ParabolicMovementComponent{
    glm::vec3 startPos;
    glm::vec3 endPos;
    float fireLerp = 0.0f;
    float height;
    float speed;
    parabolicCompletion identifier;
    float finalLerp;
    ParabolicMovementComponent() = default;
    ParabolicMovementComponent(glm::vec3 startPos, glm::vec3 endPos, float height, float speed, parabolicCompletion identifier, float finalLerp = 1.0): 
        startPos(startPos), endPos(endPos), height(height), speed(speed), identifier(identifier), finalLerp(finalLerp){}
};


#endif