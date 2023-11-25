#ifndef ROOMSPAWN_H
#define ROOMSPAWN_H

#include "enums.h"

struct roomSpawn{
    double modifier;
    sprites monster;

    inline roomSpawn(double modifier, sprites monster): modifier(modifier), monster(monster) {}

};

#endif