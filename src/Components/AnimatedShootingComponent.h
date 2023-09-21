#pragma once

#include "../Utils/tables.h"
#include "../Utils/enums.h"

struct AnimatedShootingComponent{
    unsigned char extraPixels;
    bool animatedShooting;  // this flag tells the renderSystem to increase sprite width and destRect by extraPixels !

    AnimatedShootingComponent(unsigned char extraPixels = 0, bool animatedShooting = false) : extraPixels(extraPixels), animatedShooting(animatedShooting) {}

    AnimatedShootingComponent(classes cn){
        this->extraPixels = classToAnimatedShootingData.at(cn).extraPixels;
        this->animatedShooting = false;
    }

    AnimatedShootingComponent(sprites sprite){
        this->extraPixels = spriteToAnimatedShootingData.at(sprite).extraPixels;
        this->animatedShooting = false; 
    }
};
