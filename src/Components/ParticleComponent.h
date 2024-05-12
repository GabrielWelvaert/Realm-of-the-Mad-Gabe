#ifndef PARTICLECOMPONENT_H
#define PARTICLECOMPONENT_H

#include "../ECS/ECS.h"
#include "../../libs/SDL2/SDL.h"

struct ParticleComponent{
    Uint32 deathTime;
    ParticleComponent() = default;
    ParticleComponent(Uint32 deathTime): deathTime(deathTime) {}
};

#endif