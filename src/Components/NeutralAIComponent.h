#ifndef NEUTRALAICOMPONENT_H
#define NEUTRALAICOMPONENT_H

#include "../Utils/enums.h"
#include "../Utils/tables.h"

struct NeutralAIComponent{
    unsigned short engageRange; // distance where monster will further engage if possible (ex: shoot)

    inline NeutralAIComponent() = default;

    inline NeutralAIComponent(sprites se){
        auto pec = spriteEnumToPEC.at(se);
        this->engageRange = (pec.projectileSpeed * pec.duration / 1000);
    }

    inline NeutralAIComponent(unsigned short engageRange){
        
    }
};

#endif