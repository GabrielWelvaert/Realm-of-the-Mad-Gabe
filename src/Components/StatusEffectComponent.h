#pragma once 

#include <iostream>
#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"

// statuses enum used to index effects and endTimes.
// ex: if effects[SPEEDY] then entTimes[SPEEDY] shows endTime and modifications[SPEEDY] shows stat mod applied
// not all abilities will require stat modification such as paralyze and quiet and wont use modification vector
struct StatusEffectComponent{
    std::bitset<8> effects; 
    // could also add a bitset<8> that represents immunities if I wanted to
    std::vector<Uint32> endTimes = {0,0,0,0,0,0,0,0};
    std::vector<int> modificiations = {0,0,0,0,0,0,0,0};

    StatusEffectComponent() = default;

    void set(statuses status, Uint32 durationMS){
        effects[status] = true;
        endTimes[status] = SDL_GetTicks() + durationMS;
    }

};