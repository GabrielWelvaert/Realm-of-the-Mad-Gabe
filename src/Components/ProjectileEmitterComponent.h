#ifndef PROJECTILEEMITTERCOMPONENT_H
#define PROJECTILEEMITTERCOMPONENT_H

#include "../../libs/glm/glm.hpp"
#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"
#include "../ECS/ECS.h"
#include <unordered_map>
#include "../Utils/tables.h"

// use to access PECbools
enum PECBoolEnums{ 
    PIERCING,
    DIAGONALSPRITE,
    INFLICTSSTATUSEFFECT,
    IGNORESDEFENSE,
    OSCILLATES,
    ROTATES,
    ORBITS,
    BOOMERANG
};

struct ProjectileEmitterComponent{
    std::bitset<8> PECbools; // access via PECBoolEnums

    // info about emitter
    int emitterId;
    sprites emitterSprite;

    // basic data
    unsigned short repeatFrequency; // ms per shot 
    unsigned short projectileSpeed; 
    unsigned short duration; // ms until death of projectile
    unsigned short maxDamage; // max damage 
    unsigned short minDamage; // used by player; damage between minDamage and damage;
    Uint32 lastEmissionTime;
    unsigned char shots = 0; // dont use odd number of shots with 360, 720, etc
    float arcgap; // sum of all angles between shots

    // data about the sprite of an emitted projectile
    textureEnums spriteassetId; 
    SDL_Rect spritesrcRect;

    // hitbox data
    unsigned short boxwidth;
    unsigned short boxheight;
    glm::vec2 boxoffset;

    // status effect infliction data
    statuses statusEffect;
    unsigned short durationMS;

    // oscillating projectile data
    float amplitude;
    float frequency; // not repeat frequency. oscillation stuff.

    inline ProjectileEmitterComponent() = default;

    // this constructor is used by monsters. player pec is updated in ProjectileEmitSystem::onPlayerEquippedWeapon
    inline ProjectileEmitterComponent(sprites spriteEnum, int parentId):
        emitterId(parentId), emitterSprite(spriteEnum)
        {
            const auto& pecdata = spriteEnumToPEC.at(spriteEnum);
            this->repeatFrequency = pecdata.repeatFrequency;
            this->projectileSpeed = pecdata.projectileSpeed;
            this->duration = pecdata.duration;
            this->maxDamage = pecdata.maxDamage;
            this->shots = pecdata.shots;
            this->arcgap = pecdata.arcgap;
            this->spriteassetId = pecdata.spriteassetId;
            this->spritesrcRect = pecdata.spritesrcRect;
            this->boxheight = pecdata.boxheight;
            this->boxwidth = pecdata.boxwidth;
            this->boxoffset = pecdata.boxoffset;
            this->PECbools = pecdata.PECbools;
            this->statusEffect = pecdata.statusEffect;
            this->durationMS = pecdata.durationMS;
            this->frequency = pecdata.frequency;
            this->amplitude = pecdata.amplitude;
    }
};

#endif