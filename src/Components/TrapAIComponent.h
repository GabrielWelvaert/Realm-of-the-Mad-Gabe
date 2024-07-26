#ifndef TRAPAICOMPONENT_H
#define TRAPAICOMPONENT_H

#include "../Utils/enums.h"
#include "../Utils/tables.h"

struct TrapAIComponent{
    unsigned short engageRange;
    bool igntied = false;
    unsigned char iginitionFrame; 

    inline TrapAIComponent() = default;

    inline TrapAIComponent(sprites se){
        auto data = spritetotrap.at(se);
        this->engageRange = data.engageRange;
        this->iginitionFrame = data.iginitionFrame;
    }
};

#endif