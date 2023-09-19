#pragma once 

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

    CollisionFlagComponent(): collisionFlag(NONESIDE), lastSideHitFlag(NONESIDE), lastlastSideHitflag(NONESIDE), msLastCollisionFlag(0), idOfWallHit(0), idOfOldWallHit(0) {}

};