#ifndef DEADPLAYER_H
#define DEADPLAYER_H

#include "enums.h"

/*this struct contains info used by game when player dies*/

struct DeadPlayer{
    classes className;
    int level;
    sprites murderer;
    int xp;

    inline DeadPlayer() = default;
    inline DeadPlayer(classes className, int level, sprites murderer, int xp):
        className(className), level(level), murderer(murderer), xp(xp) {}
};

#endif