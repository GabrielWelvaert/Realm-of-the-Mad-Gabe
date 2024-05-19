#ifndef MINIONCOMPONENT_H
#define MINIONCOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"

struct MinionComponent{
    minionAItypes minionType;
    int idOfParent;
    unsigned int creationIdOfParent;
    MinionComponent() = default;
    MinionComponent(minionAItypes minionType, int idOfParent, unsigned int creationIdOfParent):
        minionType(minionType), idOfParent(idOfParent), creationIdOfParent(creationIdOfParent) {}
};

#endif