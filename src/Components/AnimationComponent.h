#pragma once
// #include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"
#include "../Utils/tables.h"
// #include "../Utils/factory.h"

struct AnimationComponent{
    long numFrames;
    unsigned char currentFrame;
    double frameSpeedRate;
    Uint32 startTime;
    unsigned char xmin;

    AnimationComponent(int numFrames = 1, int frameSpeedRate = 1,  int xmin = 1){
        this->numFrames = numFrames;
        this->frameSpeedRate = frameSpeedRate;
        this->startTime = SDL_GetTicks();
        this->xmin = xmin;
        this->currentFrame = xmin;
    }

    AnimationComponent(sprites spriteEnum){
        auto data = spriteToAnimationData.at(spriteEnum);
        this->numFrames = data.numFrames;
        this->xmin = data.xmin;
        this->currentFrame = xmin;
        auto pecrf = spriteEnumToPEC.at(spriteEnum).repeatFrequency;
        // this->frameSpeedRate = ((1000 / pecrf - 1.5) + 1.5) * 2;
        this->frameSpeedRate = 2000 / pecrf;
    }

};
