#ifndef ABILITYCOMPONENT_H
#define ABILITYCOMPONENT_H

#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"
/*
Ability Component has data used by all abilities. other components are unique (semantically) to each category
*/
struct AbilityComponent{
    bool abilityEquipped;
    Uint32 timeLastUsed;
    int mpRequired = 20;
    int coolDownMS = 500;
    bool blockNoManaSound = false;

    inline AbilityComponent() = default;

    inline AbilityComponent(int mpRequired, int coolDownMS):
        abilityEquipped(false), 
        timeLastUsed(0), 
        mpRequired(mpRequired), 
        coolDownMS(coolDownMS) {}
};

struct TomeComponent{
    int hp;
    items tomeEnum;

    inline TomeComponent() = default;

    inline TomeComponent(int hp, items tomeEnum): hp(hp), tomeEnum(tomeEnum) {} 

};

struct ShieldComponent{
    statuses debuff;
    int debuffDuration;
    int numshots;
    int minDamage;
    int maxDamage;
    items shieldEnum;

    inline ShieldComponent() = default;
    inline ShieldComponent(statuses debuff, int debuffDuration, int numshots, int minDamage, int maxDamage, items shieldEnum): 
        debuff(debuff), debuffDuration(debuffDuration), numshots(numshots), minDamage(minDamage), maxDamage(maxDamage), shieldEnum(shieldEnum) {}
};

struct SpellComponent{
    int minDamage;
    int maxDamage;
    textureEnums texture;
    SDL_Rect srcRect;
    items spellEnum;

    inline SpellComponent() = default;
    inline SpellComponent(int minDamage, int maxDamage, textureEnums texture, SDL_Rect srcRect, items spellEnum):
        minDamage(minDamage), maxDamage(maxDamage), texture(texture), srcRect(srcRect), spellEnum(spellEnum) {}
};

struct CloakComponent{
    int invisibilityDuration;
    items itemEnum;

    inline CloakComponent() = default;

    inline CloakComponent(int invisibilityDuration, items itemEnum): invisibilityDuration(invisibilityDuration), itemEnum(itemEnum) {}
};

struct HelmComponent{
    int berserkDuration;
    items itemEnum;

    inline HelmComponent() = default;

    inline HelmComponent(int berserkDuration, items itemEnum): berserkDuration(berserkDuration), itemEnum(itemEnum) {}
};

struct QuiverComponent{
    int minDamage;
    int maxDamage;
    textureEnums texture;
    SDL_Rect srcRect;
    statuses debuff;
    items quiverEnum;

    inline QuiverComponent() = default;

    inline QuiverComponent(int minDamage, int maxDamage): minDamage(minDamage), maxDamage(maxDamage), texture(LOFIPROJS), srcRect{0,0,0,0} {}

};

struct SealComponent{
    int duration;
    items itemEnum;
    inline SealComponent() = default;
    inline SealComponent(int duration, items itemEnum): duration(duration), itemEnum(itemEnum) {}
};

struct ScepterComponent{
    int maxNumberTargets;
    int damage;
    int damageReductionPerTarget;
    inline ScepterComponent() = default;
    inline ScepterComponent(int maxNumberTargets,int damage,int damageReductionPerTarget):
        maxNumberTargets(maxNumberTargets), damage(damage), damageReductionPerTarget(damageReductionPerTarget) {}
};

struct SkullComponent{
    int damage;
    float radius;
    items itemEnum;
    inline SkullComponent() = default;
    inline SkullComponent(int damage, float radius, items itemEnum):
        damage(damage), radius(radius), itemEnum(itemEnum) {}
};


#endif