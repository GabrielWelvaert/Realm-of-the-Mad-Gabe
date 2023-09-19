#ifndef BASESTATCOMPONENT_H
#define BASESTATCOMPONENT_H

#include <vector>
#include <unordered_map>
#include "../Utils/enums.h"

struct BaseStatData{
    unsigned short hp;
    unsigned short mp;
    unsigned char attack;
    unsigned char defense;
    unsigned char speed;
    unsigned char dexterity;
    unsigned char vitality;
    unsigned char wisdom;
};

std::unordered_map<classes, BaseStatData> classToBaseStats = {
    {ARCHER, {130,100,12,0,12,12,12,10}},
    {WARRIOR, {200,100,15,0,7,10,10,10}},
    {PRIEST, {100,100,12,0,12,12,10,15}}
};

// base stat components are only used by player 
// they're updated at leveling up and using stat pots
struct BaseStatComponent{
    unsigned short hp;
    unsigned short mp;
    unsigned char attack;
    unsigned char defense;
    unsigned char speed;
    unsigned char dexterity;
    unsigned char vitality;
    unsigned char wisdom;
    unsigned char level;
    unsigned int xp;

    BaseStatComponent()
        : hp(0), mp(0), attack(0), defense(0), speed(0),
          dexterity(0), vitality(0), wisdom(0), level(0), xp(0) {}

    BaseStatComponent(classes classname) : 
        hp(classToBaseStats.at(classname).hp),
        mp(classToBaseStats.at(classname).mp),
        attack(classToBaseStats.at(classname).attack),
        defense(classToBaseStats.at(classname).defense),
        speed(classToBaseStats.at(classname).speed),
        dexterity(classToBaseStats.at(classname).dexterity),
        vitality(classToBaseStats.at(classname).vitality),
        wisdom(classToBaseStats.at(classname).wisdom),
        level(1), xp(0) {}

    //TODO constructor for loading existing character?



};

#endif