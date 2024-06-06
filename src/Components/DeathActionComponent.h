#ifndef DEATHACTIONCOMPONENT_H
#define DEATHACTIONCOMPONENT_H

#include "../Utils/enums.h"

struct DeathActionComponent{
    sprites spriteEnum;
    DeathActionComponent() = default;
    DeathActionComponent(sprites spriteEnum):
        spriteEnum(spriteEnum) {}
};

#endif