#ifndef SECONDARYPROJECTILE_H
#define SECONDARYPROJECTILE_H

#include "../Utils/tables.h"
#include "../../libs/SDL2/SDL.h"

struct SecondaryProjectileComponent{

    unsigned long repeatFrequency;
    Uint32 lastEmissionTime;

    inline SecondaryProjectileComponent() = default;
    
    inline SecondaryProjectileComponent(unsigned long repeatFrequency):
        repeatFrequency(repeatFrequency), lastEmissionTime(SDL_GetTicks() - repeatFrequency) {}

};


#endif