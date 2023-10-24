#ifndef SPEEDSTATCOMPONENT_H
#define SPEEDSTATCOMPONENT_H

#include "../Components/BaseStatComponent.h"
#include "../Utils/enums.h"

struct SpeedStatComponent{
    unsigned char activespeed;

    inline SpeedStatComponent() : activespeed(0) {}

    inline SpeedStatComponent(unsigned char speed): activespeed(speed) {}

    inline SpeedStatComponent(BaseStatComponent bsc) : activespeed(bsc.speed) {}

    inline SpeedStatComponent(sprites spriteEnum){
        statData data = spriteEnumToStatData.at(spriteEnum);
        this->activespeed = data.activespeed;
    }
};

#endif 