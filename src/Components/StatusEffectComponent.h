#ifndef STATUSEFFECTCOMPONENT_H
#define STATUSEFFECTCOMPONENT_H

#include <iostream>
#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"

// statuses enum used to index effects and endTimes.
// ex: if effects[SPEEDY] then endTimes[SPEEDY] shows endTime and modifications[SPEEDY] shows stat mod applied
// not all abilities will require stat modification such as paralyze and quiet and thus wont use modification vector
struct StatusEffectComponent{
    std::bitset<8> effects; 
    // std::bitset<8> immunities // how i'd do immunities, if I wanted to do them 
    std::vector<Uint32> endTimes = {0,0,0,0,0,0,0,0};
    std::vector<int> modifications = {0,0,0,0,0,0,0,0};
    Uint32 lastBleedTime = 0; // floating point precision problems w/ -= .02hp * deltaTime, so this is solution (bleed every 250ms)

    inline StatusEffectComponent() = default;

    inline void set(statuses status, Uint32 durationMS){
        effects[status] = true;
        endTimes[status] = SDL_GetTicks() + durationMS;
    }

};

#endif