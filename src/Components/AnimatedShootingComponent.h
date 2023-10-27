#ifndef ANIMATEDSHOOTINGCOMPONENT_H
#define ANIMATEDSHOOTINGCOMPONENT_H

#include "../Utils/tables.h"
#include "../Utils/enums.h"

struct AnimatedShootingComponent{
    unsigned char extraPixels;
    bool animatedShooting;  // this flag tells the renderSystem to increase sprite width and destRect by extraPixels !

    inline AnimatedShootingComponent(unsigned char extraPixels = 0, bool animatedShooting = false) : extraPixels(extraPixels), animatedShooting(animatedShooting) {}

    inline AnimatedShootingComponent(classes cn):
        extraPixels(classToAnimatedShootingData.at(cn).extraPixels),
        animatedShooting(false)
        {}

    inline AnimatedShootingComponent(sprites sprite):
        extraPixels(spriteToAnimatedShootingData.at(sprite).extraPixels),
        animatedShooting(false)
        {}
        
};

#endif