#pragma once 

#include "../Utils/enums.h"
#include "../Utils/tables.h"

struct AnimatedNeutralAIComponent{
    unsigned short engageRange; // distance where monster will further engage if possible (ex: shoot)
    
    AnimatedNeutralAIComponent() = default;

    AnimatedNeutralAIComponent(sprites se){
        auto pec = spriteEnumToPEC.at(se);
        this->engageRange = (pec.projectileSpeed * pec.duration / 1000);
    }
};