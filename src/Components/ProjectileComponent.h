#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include <unordered_set>
#include "../Utils/enums.h"
#include "../ECS/ECS.h"


struct ProjectileComponent {
    Entity parent;
    short damage;
    unsigned short duration;
    Uint32 startTime;
    bool piercing;
    unsigned char parentGroupEnumInt;

    bool inflictsStatusEffect = false;
    statuses statsusEffect = QUIET;
    unsigned short SEdurationMS = 0;
    sprites spriteOfParent;
    
    bool ignoresDefense = false;

    inline ProjectileComponent() = default;

    inline ProjectileComponent(
        int damage, 
        int duration, 
        bool piercing, 
        Entity parent, 
        unsigned char parentGroupEnumInt, 
        sprites spriteOfParent, 
        bool inflictsStatusEffect = false,  
        statuses statsusEffect = QUIET, 
        unsigned short SEdurationMS = 0, 
        bool ignoresDefense = false): 

        damage(damage), 
        duration(duration), 
        piercing(piercing), 
        parent(parent), 
        spriteOfParent(spriteOfParent),
        startTime(SDL_GetTicks()), 
        parentGroupEnumInt(parentGroupEnumInt),
        inflictsStatusEffect(inflictsStatusEffect),
        statsusEffect(statsusEffect),
        SEdurationMS(SEdurationMS),
        ignoresDefense(ignoresDefense)
        {
            
        } 
};

#endif