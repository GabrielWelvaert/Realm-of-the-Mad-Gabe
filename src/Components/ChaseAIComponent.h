#ifndef CHASEAICOMPONENT_H
#define CHASEAICOMPONENT_H

#include "../Utils/enums.h"
#include "../Utils/tables.h"
#include "../Utils/Xoshiro256.h"

struct ChaseAIComponent{
    unsigned short detectRange; // distance in pixels where monster can detect player (ex: path to player)
    unsigned short engageRange; // distance where monster will further engage if possible (ex: shoot)
    unsigned short maxDistance; // closest distance in pixels monster wants to get to player (so monster doens't stand on top of player, or if so they do)

    inline ChaseAIComponent() = default;

    inline ChaseAIComponent(sprites se, Xoshiro256& RNG){
        auto data = spritesToaiChaseData.at(se);
        auto pec = spriteEnumToPEC.at(se);
        this->engageRange = (pec.projectileSpeed * pec.duration / 1000);
        this->maxDistance = RNG.randomSmediumModification(data.maxDistance);
        data.detectRange < engageRange ? this->detectRange = engageRange : this->detectRange = data.detectRange;
    }
};

#endif