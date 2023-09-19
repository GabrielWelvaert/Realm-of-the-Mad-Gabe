#pragma once 

#include "../Utils/enums.h"
#include "../Utils/tables.h"

struct TrapAIComponent{
    unsigned short engageRange;
    bool igntied;
    unsigned char iginitionFrame; 

    TrapAIComponent() = default;

    TrapAIComponent(sprites se){
        auto data = spritetotrap.at(se);
        this->engageRange = data.engageRange;
        this->iginitionFrame = data.iginitionFrame;
    }
};