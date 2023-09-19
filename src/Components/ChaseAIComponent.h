#pragma once 

#include "../Utils/enums.h"
#include "../Utils/tables.h"

struct ChaseAIComponent{
    unsigned short detectRange; // distance in pixels where monster can detect player (ex: path to player)
    unsigned short engageRange; // distance where monster will further engage if possible (ex: shoot)
    unsigned short maxDistance; // closest distance in pixels monster wants to get to player (so monster doens't stand on top of player, or if so they do)

    ChaseAIComponent() = default;

    ChaseAIComponent(sprites se){
        auto data = spritesToaiChaseData.at(se);
        auto pec = spriteEnumToPEC.at(se);
        this->engageRange = (pec.projectileSpeed * pec.duration / 1000);
        this->maxDistance = data.maxDistance;
        data.detectRange < engageRange ? this->detectRange = engageRange : this->detectRange = data.detectRange;
    }
};