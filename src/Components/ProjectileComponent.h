#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include <unordered_set>

struct ProjectileComponent {
    Entity parent;
    short damage;
    unsigned short duration;
    Uint32 startTime;
    bool piercing;
    unsigned char parentGroupEnumInt;

    ProjectileComponent() = default;

    ProjectileComponent(int damage, int duration, bool piercing, Entity parent, unsigned char parentGroupEnumInt): 
        damage(damage), 
        duration(duration), 
        piercing(piercing), 
        parent(parent), 
        // not an argument!:
        startTime(SDL_GetTicks()), 
        parentGroupEnumInt(parentGroupEnumInt)
        {} 
};

#endif