#ifndef COLLISIONFLAGCOMPONENT_H
#define COLLISIONFLAGCOMPONENT_H

#include "../../libs/glm/glm.hpp"
#include "../Utils/enums.h"
#include "../../libs/SDL2/SDL.h"

struct CollisionFlagComponent{
    sidehit collisionFlag;
    sidehit lastSideHitFlag;
    sidehit lastlastSideHitflag;
    Uint32 msLastCollisionFlag;
    int idOfWallHit;
    int idOfOldWallHit; 

    inline CollisionFlagComponent(): 
        collisionFlag(NONESIDE), 
        lastSideHitFlag(NONESIDE), 
        lastlastSideHitflag(NONESIDE), 
        msLastCollisionFlag(0), 
        idOfWallHit(-1), 
        idOfOldWallHit(-1) {
            // std::cout << "collisionFlagComponent constructor called" << std::endl; // this constructor gets called a ton for some reason when loading player!
        }

};

#endif