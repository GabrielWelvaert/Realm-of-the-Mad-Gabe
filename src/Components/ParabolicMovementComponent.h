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
    sprites sprite;
    ParabolicMovementComponent() = default;
    ParabolicMovementComponent(glm::vec3 startPos, glm::vec3 endPos, float height, float speed, parabolicCompletion identifier, sprites sprite = MEDUSA): 
        startPos(startPos), endPos(endPos), height(height), speed(speed), identifier(identifier), finalLerp(1.0), sprite(sprite){}
};


#endif