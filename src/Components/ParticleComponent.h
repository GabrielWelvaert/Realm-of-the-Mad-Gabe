#ifndef PARTICLECOMPONENT_H
#define PARTICLECOMPONENT_H

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"

struct ParticleComponent{
    Uint32 deathTime;
    bool shrinking;
    Uint32 timeOflastShrink = 0;
    ParticleComponent() = default;
    ParticleComponent(Uint32 deathTime, bool shrinking = false):
         deathTime(deathTime), shrinking(shrinking) {}
};

#endif