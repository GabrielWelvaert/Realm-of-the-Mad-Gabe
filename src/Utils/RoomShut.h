#ifndef ROOMSHUT_H
#define ROOMSHUT_H

#include "enums.h"
#include "../../libs/glm/glm.hpp"
#include <vector>
/*this struct contains data needed to lock player in a bossroom*/

struct RoomShut{
    cardinalDirection directionOfHallway;
    glm::ivec2 coordiantes;
    
    inline RoomShut() = default;
    inline RoomShut(cardinalDirection directionOfHallway, glm::ivec2 coordiantes):
        directionOfHallway(directionOfHallway), coordiantes(coordiantes) {}
};

#endif
