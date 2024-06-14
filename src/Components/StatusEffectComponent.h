#ifndef STATUSEFFECTCOMPONENT_H
#define STATUSEFFECTCOMPONENT_H

#include <iostream>
#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"

// statuses enum used to index effects and endTimes.
// ex: if effects[SPEEDY] then endTimes[SPEEDY] shows endTime and modifications[SPEEDY] shows stat mod applied
// not all abilities will require stat modification
struct StatusEffectComponent{
    std::bitset<TOTAL_NUMBER_OF_STATUS_EFFECTS - 1> effects; 
    // std::bitset<8> immunities // how i'd do immunities, if I wanted to do them 
    std::vector<Uint32> endTimes = std::vector<Uint32>(TOTAL_NUMBER_OF_STATUS_EFFECTS - 1);
    std::vector<int> modifications = std::vector<int>(TOTAL_NUMBER_OF_STATUS_EFFECTS - 1);
    Uint32 lastBleedTime = 0; // floating point precision problems w/ -= .02hp * deltaTime, so this is solution (bleed every 250ms)
    Uint32 lastHealTime = 0;

    inline StatusEffectComponent() = default;

    inline void set(statuses status, Uint32 durationMS){
        effects[status] = true;
        endTimes[status] = SDL_GetTicks() + durationMS;
    }

};

#endif