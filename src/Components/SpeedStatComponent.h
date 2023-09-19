#ifndef SPEEDSTATCOMPONENT_H
#define SPEEDSTATCOMPONENT_H

#include "../Components/BaseStatComponent.h"
#include "../Utils/enums.h"

struct SpeedStatComponent{
    unsigned char activespeed;

    SpeedStatComponent() : activespeed(0) {}

    SpeedStatComponent(unsigned char speed): activespeed(speed) {}

    SpeedStatComponent(BaseStatComponent bsc) : activespeed(bsc.speed) {}

    SpeedStatComponent(sprites spriteEnum){
        statData data = spriteEnumToStatData.at(spriteEnum);
        this->activespeed = data.activespeed;
    }
};

#endif 