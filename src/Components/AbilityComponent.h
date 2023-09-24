#pragma once 

#include "../../libs/SDL2/SDL.h"
#include "../Utils/enums.h"
/*
Each ability inherits from parent ability class and abilities are used via their respective events
*/
struct AbilityComponent{
    bool abilityEquipped;
    Uint32 timeLastUsed;
    int mpRequired;
    int coolDownMS;

    AbilityComponent() = default;

    AbilityComponent(int mpRequired, int coolDownMS):
        abilityEquipped(false), 
        timeLastUsed(0), 
        mpRequired(mpRequired), 
        coolDownMS(coolDownMS) {}
};

struct TomeComponent{
    int hp;

    TomeComponent() = default;

    TomeComponent(int hp): hp(hp) {} 

};

struct HelmComponent{
    int berserkDuration;

    HelmComponent() = default;

    HelmComponent(int berserkDuration): berserkDuration(berserkDuration) {}
};

struct QuiverComponent{
    int minDamage;
    int maxDamage;
    textureEnums texture;
    SDL_Rect srcRect;

    QuiverComponent() = default;

    QuiverComponent(int minDamage, int maxDamage): minDamage(minDamage), maxDamage(maxDamage), texture(LOFIPROJS), srcRect{0,0,0,0} {}

};
