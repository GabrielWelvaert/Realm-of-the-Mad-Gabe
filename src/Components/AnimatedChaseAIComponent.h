#ifndef ANIMATEDCHASEAICOMPONENT_H
#define ANIMATEDCHASEAICOMPONENT_H

#include "../Utils/enums.h"
#include "../Utils/tables.h"
#include "../Utils/Xoshiro256.h"

struct AnimatedChaseAIComponent{
    unsigned short detectRange; // distance in pixels where monster can detect player (ex: path to player)
    unsigned short engageRange; // distance where monster will further engage if possible (ex: shoot)
    unsigned short maxDistance; // closest distance in pixels monster wants to get to player (so monster doens't stand on top of player, or if so they do)

    inline AnimatedChaseAIComponent() = default;

    inline AnimatedChaseAIComponent(sprites se, Xoshiro256& RNG){
        const auto& data = spritesToaiChaseData.at(se);
        this->maxDistance = RNG.randomSmediumModification(data.maxDistance);
        const auto& pec = spriteEnumToPEC.at(se);
        this->engageRange = (pec.projectileSpeed * pec.duration / 1000);
        data.detectRange < engageRange ? this->detectRange = engageRange : this->detectRange = data.detectRange;
    }

    
};

#endif