#ifndef ROTATIONCOMPONENT_H
#define ROTATIONCOMPONENT_H

#include "../../libs/SDL2/SDL.h"

struct RotationComponent{

    float AngleChangePerDeltaTime; 
    
    RotationComponent() = default;

    // pass RPS, it works like that I swear
    RotationComponent(float AngleChangePerDeltaTime): AngleChangePerDeltaTime(AngleChangePerDeltaTime * 100) {}

};

#endif