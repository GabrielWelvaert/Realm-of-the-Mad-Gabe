#ifndef PROJECTILEEMITTERCOMPONENT_H
#define PROJECTILEEMITTERCOMPONENT_H

#include "../../libs/glm/glm.hpp"
#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"
#include "../ECS/ECS.h"
#include <unordered_map>
#include "../Utils/tables.h"

// use to access PECbools
// enum PECBoolEnums{ 
//     PIERCING,
//     DIAGONALSPRITE,
//     INFLICTSSTATUSEFFECT,
//     IGNORESDEFENSE,
//     OSCILLATES,
// };

// struct ProjectilEmitterComponent{
//     std::bitset<8> PECbools; // access via PECBoolEnums

//     // Entity field should just be ID (ProjectileDamageEvent already has registry)
//     // fixed sprite can be removed. projectiles are never fixed
//     // zindex can be removed. all sprites have same zindex
//     // width and height fields can be removed from the sprite data portion

//     // info about emitter
//     int emitterId;
//     sprites emitterSprite;

//     // basic data
//     unsigned short repeatFrequency; // ms per shot 
//     unsigned short projectileSpeed; 
//     unsigned short duration; // ms until death of projectile
//     unsigned short maxDamage; // max damage 
//     unsigned short minDamage; // used by player; damage between minDamage and damage;
//     unsigned short projectileSpeed; 
//     Uint32 lastEmissionTime;
//     unsigned char shots = 0; // dont use odd number of shots with 360, 720, etc
//     float arcgap; // sum of all angles between shots

//     // data about the sprite of an emitted projectile
//     textureEnums spriteassetId; 
//     SDL_Rect spritesrcRect;

//     // hitbox data
//     unsigned short boxwidth;
//     unsigned short boxheight;
//     glm::vec2 boxoffset;

//     // status effect infliction data
//     statuses statusEffect;
//     unsigned short durationMS;

//     // oscillating projectile data
//     float amplitude;
//     float frequency; // not repeat frequency. oscillation stuff.
//     // float additionalOffset; // remove? never used?
// };

struct ProjectileEmitterComponent{
    // bool isShooting; // this is now its own compoennt for cache friendliness
    unsigned short repeatFrequency; // ms per shot 
    unsigned short duration; // ms until death of projectile
    Entity parent;  // parent
    unsigned short damage; // max damage 
    unsigned short minDamage; // used by player; damage between minDamage and damage;
    unsigned short projectileSpeed;  
    Uint32 lastEmissionTime; // last time we shot a projectile from SDL_GetTicks()
    bool piercing;
    unsigned char shots = 0; // dont use odd number of shots with 360, 720, etc
    float arcgap; 

    // data about projectile's sprite 
    textureEnums spriteassetId; // texture enum used in assetStore
    unsigned char spritewidth;
    unsigned char spriteheight;
    SDL_Rect spritesrcRect;
    unsigned char spritezIndex;
    bool spriteisFixed; // fixed sprites stay in same spot on screen 
    bool spritediagonalSprite;

    // data about projectile's hitbox
    unsigned short boxwidth;
    unsigned short boxheight;
    glm::vec2 boxoffset;

    // data about status effect infliction 
    bool inflictsStatusEffect = false;
    statuses statusEffect = QUIET;
    unsigned short durationMS = 0;

    //sprites 
    sprites spriteOfParent;

    //does it pierce
    bool ignoresDefense = false;

    // does it oscillate
    bool oscillation;
    float amplitude;
    float frequency; // not repeat frequency. oscillation stuff.
    float additionalOffset;

    inline ProjectileEmitterComponent() = default;

    inline ProjectileEmitterComponent(Entity parent,bool isShooting,unsigned short repeatFrequency,unsigned short duration, unsigned short damage,unsigned short projectileSpeed,bool piercing,unsigned char shots,float arcgap,textureEnums spriteassetId,unsigned char spritewidth,unsigned char spriteheight,SDL_Rect spritesrcRect,unsigned char spritezIndex,bool spriteisFixed,bool spritediagonalSprite,unsigned short boxwidth,unsigned short boxheight,glm::vec2 boxoffset) : 
        parent(parent),
        // isShooting(isShooting),
        repeatFrequency(repeatFrequency),
        duration(duration),
        damage(damage),
        projectileSpeed(projectileSpeed),
        lastEmissionTime(SDL_GetTicks() - repeatFrequency),
        piercing(piercing),
        shots(shots),
        arcgap(arcgap),
        spriteassetId(spriteassetId),
        spritewidth(spritewidth),
        spriteheight(spriteheight),
        spritesrcRect(spritesrcRect),
        spritezIndex(spritezIndex),
        spriteisFixed(spriteisFixed),
        spritediagonalSprite(spritediagonalSprite),
        boxwidth(boxwidth),
        boxheight(boxheight),
        boxoffset(boxoffset){}

    // used by monsters
    inline ProjectileEmitterComponent(sprites spriteEnum, Entity self){
        const auto& pecdata = spriteEnumToPEC.at(spriteEnum);
        this->parent = self;
        this->duration = pecdata.duration; 
        this->damage = pecdata.damage;
        // this->isShooting = pecdata.isShooting;
        this->repeatFrequency = pecdata.repeatFrequency;
        this->projectileSpeed = pecdata.projectileSpeed;
        this->piercing = pecdata.piercing;
        this->shots = pecdata.shots;
        this->arcgap = pecdata.arcgap;
        this->spriteassetId = pecdata.spriteassetId;
        this->spritewidth = pecdata.spritewidth;
        this->spriteheight = pecdata.spriteheight;
        this->spritesrcRect = pecdata.spritesrcRect;
        this->spritezIndex = pecdata.spritezIndex;
        this->spriteisFixed = pecdata.spriteisFixed;
        this->spritediagonalSprite = pecdata.spritediagonalSprite;
        this->boxwidth = pecdata.boxwidth;
        this->boxheight = pecdata.boxheight;
        this->boxoffset = pecdata.boxoffset;
        this->spriteOfParent = spriteEnum;
        this->ignoresDefense = pecdata.ignoresDefense;
        this->inflictsStatusEffect = pecdata.inflictsStatusEffect;
        this->statusEffect = pecdata.statusEffect;
        this->durationMS = pecdata.durationMS;
        this->oscillation = pecdata.oscillation;
        this->frequency = pecdata.frequency;
        this->amplitude = pecdata.amplitude;
    }


};

#endif