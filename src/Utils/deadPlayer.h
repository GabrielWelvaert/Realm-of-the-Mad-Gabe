#ifndef DEADPLAYER_H
#define DEADPLAYER_H

#include "enums.h"

/*this struct contains info used by game when player dies*/

struct deadPlayer{
    classes className;
    int level;
    sprites murderer;
    int xp;

    inline deadPlayer() = default;
    inline deadPlayer(classes className, int level, sprites murderer, int xp):
        className(className), level(level), murderer(murderer), xp(xp) {}
};

#endif