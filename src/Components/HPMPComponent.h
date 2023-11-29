#ifndef HPMPCOMPONENT_H
#define HPMPCOMPONENT_H

#include "../Components/BaseStatComponent.h"
#include "../Utils/enums.h"
#include "../Utils/tables.h"
// #include "../Utils/factory.h"

struct HPMPComponent{
    double activehp; // if < 0 you die 
    float activemp;
    unsigned short maxhp; //modified by rings and shit
    unsigned short maxmp;
    unsigned char activewisdom;
    unsigned char activevitality;
    unsigned char activedefense;
    soundEnums hitsound;
    soundEnums deathsound;

    inline HPMPComponent() 
        : activehp(0), activemp(0), maxhp(0), maxmp(0), activewisdom(0), activevitality(0), activedefense(0), hitsound(DEFAULTHIT), deathsound(DEFAULTDEATH) {
    }

    inline HPMPComponent(double activehp, float activemp, unsigned short maxhp, unsigned short maxmp, unsigned char activewisdom, unsigned char activevitality, unsigned char activedefense, soundEnums hitsound, soundEnums deathsound)
        : activehp(activehp), activemp(activemp), maxhp(maxhp), maxmp(maxmp), activewisdom(activewisdom), activevitality(activevitality), activedefense(activedefense), hitsound(hitsound), deathsound(deathsound) {
    }

    // passing spriteEnum, for spawning monsters (note: only used fields are initialized!)
    inline HPMPComponent(sprites spriteEnum){
        statData data = spriteEnumToStatData.at(spriteEnum);
        this->activehp = data.activehp;
        this->maxhp = data.activehp;
        this->activevitality = data.activevitality;
        this->activedefense = data.activedefense;
        this->hitsound = data.hitsound;
        this->deathsound = data.deathsound;
    }

    // constructor with baseStat argument used for player only!
    inline HPMPComponent(BaseStatComponent basestats)
        : activehp(basestats.hp), activemp(basestats.mp), maxhp(basestats.hp), maxmp(basestats.mp), activewisdom(basestats.wisdom), activevitality(basestats.vitality), activedefense(basestats.defense) {
    }
};

#endif