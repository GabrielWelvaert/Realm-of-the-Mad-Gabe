#ifndef PROJECTILEEMITTERCOMPONENT_H
#define PROJECTILEEMITTERCOMPONENT_H

#include "../../libs/glm/glm.hpp"
#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"
#include "../ECS/ECS.h"
#include <unordered_map>
#include "../Utils/tables.h"

struct ProjectileEmitterComponent{
    bool isShooting; 
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

    inline ProjectileEmitterComponent() = default;

    inline ProjectileEmitterComponent(Entity parent,bool isShooting,unsigned short repeatFrequency,unsigned short duration, unsigned short damage,unsigned short projectileSpeed,bool piercing,unsigned char shots,float arcgap,textureEnums spriteassetId,unsigned char spritewidth,unsigned char spriteheight,SDL_Rect spritesrcRect,unsigned char spritezIndex,bool spriteisFixed,bool spritediagonalSprite,unsigned short boxwidth,unsigned short boxheight,glm::vec2 boxoffset) : 
        parent(parent),
        isShooting(isShooting),
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

    inline ProjectileEmitterComponent(int repeatFrequency, int duration, Entity parent, int damage, int lastEmissionTime, bool isShooting){
        this->repeatFrequency = repeatFrequency; // old default of 100
        this->duration = duration; // old default of 50000
        this->parent = parent;
        this->damage = damage;
        this->lastEmissionTime = SDL_GetTicks() - repeatFrequency; // subtract repeatFrequency to fix not being able to shoot instantly after spawning?
        this->isShooting = false;
    }

    // used by monsters
    inline ProjectileEmitterComponent(sprites spriteEnum, Entity self){
        auto pecdata = spriteEnumToPEC.at(spriteEnum);
        this->parent = self;
        this->duration = pecdata.duration; 
        this->damage = pecdata.damage;
        this->isShooting = pecdata.isShooting;
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
    }

    // used for player. updates will occur when equipping items 
    inline ProjectileEmitterComponent(Entity parent, boxColliderData bcdata, spritedata sdata){ //used for player, later equipping items will manually update PEC enums 
        this->repeatFrequency = 250; // old default of 100
        this->duration = 500; // old default of 50000
        this->parent = parent;
        this->damage = 1;
        this->lastEmissionTime = SDL_GetTicks() - repeatFrequency; // subtract repeatFrequency to fix not being able to shoot instantly after spawning?
        this->spriteassetId = sdata.assetId;
        this->spritewidth = sdata.width;
        this->spriteheight = sdata.height;
        this->spritesrcRect = sdata.srcRect;
        this->spritezIndex = sdata.zIndex;
        this->spriteisFixed = sdata.isFixed;
        this->spritediagonalSprite = sdata.diagonalSprite;
        this->boxwidth = bcdata.width;
        this->boxheight = bcdata.height;
        this->boxoffset = bcdata.offset;
        this->isShooting = false;
    }


};

#endif