#ifndef BASESTATCOMPONENT_H
#define BASESTATCOMPONENT_H

#include <vector>
#include <unordered_map>
#include "../Utils/enums.h"
#include "../Utils/tables.h"

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

    inline BaseStatComponent()
        : hp(0), mp(0), attack(0), defense(0), speed(0),
          dexterity(0), vitality(0), wisdom(0), level(0), xp(0) {}

    inline BaseStatComponent(int hp, int mp, int att, int def, int spd, int dex, int vit, int wis, int level, int xp):
        hp(hp), mp(mp), attack(att), defense(def), speed(spd), dexterity(dex), vitality(vit), wisdom(wis), level(level), xp(xp) {}

    inline BaseStatComponent(classes classname) : 
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