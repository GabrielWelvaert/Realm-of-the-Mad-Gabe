#ifndef TABLES_H
#define TABLES_H

#include <unordered_map>
#include <array>
#include "enums.h"
#include <vector>
#include "../../libs/glm/glm.hpp"
#include "../../libs/SDL2/SDL.h"
#include <memory>
#include <map>
#include <string>
#include "../Utils/roomSpawn.h"
#include "../Utils/colors.h"
#include <unordered_set>
#include <bitset>

#define T1WEAPONS {T1SWORD, T1DAGGER, T1BOW, T1WAND, T1STAFF}
#define T2WEAPONS {T2SWORD, T2DAGGER, T2BOW, T2WAND, T2STAFF}
#define T3WEAPONS {T3SWORD, T3DAGGER, T3BOW, T3WAND, T3STAFF}
#define T4WEAPONS {T4SWORD, T4DAGGER, T4BOW, T4WAND, T4STAFF}
#define T5WEAPONS {T5SWORD, T5DAGGER, T5BOW, T5WAND, T5STAFF}
#define T6WEAPONS {T6SWORD, T6DAGGER, T6BOW, T6WAND, T6STAFF}
#define T7WEAPONS {T7SWORD, T7DAGGER, T7BOW, T7WAND, T7STAFF}
#define T8WEAPONS {T8SWORD, T8DAGGER, T8BOW, T8WAND, T8STAFF}
#define T9WEAPONS {T9SWORD, T9DAGGER, T9BOW, T9WAND, T9STAFF}
#define T10WEAPONS {T10SWORD, T10DAGGER, T10BOW, T10WAND, T10STAFF}
#define T11WEAPONS {T11SWORD, T11DAGGER, T11BOW, T11WAND, T11STAFF}
#define T12WEAPONS {T12SWORD, T12DAGGER, T12BOW, T12WAND, T12STAFF}
#define T13WEAPONS {T13SWORD, T13DAGGER, T13BOW, T13WAND, T13STAFF}
#define T14WEAPONS {T14SWORD, T14DAGGER, T14BOW, T14WAND, T14STAFF}

#define T1ABILITIES {T1HELM, T1SHIELD, T1SPELL, T1CLOAK, T1TOME, T1QUIVER, T1SCEPTER, T1SKULL, T1SEAL}
#define T2ABILITIES {T2HELM, T2SHIELD, T2SPELL, T2CLOAK, T2TOME, T2QUIVER, T2SCEPTER, T2SKULL, T2SEAL}
#define T3ABILITIES {T3HELM, T3SHIELD, T3SPELL, T3CLOAK, T3TOME, T3QUIVER, T3SCEPTER, T3SKULL, T3SEAL}
#define T4ABILITIES {T4HELM, T4SHIELD, T4SPELL, T4CLOAK, T4TOME, T4QUIVER, T4SCEPTER, T4SKULL, T4SEAL}
#define T5ABILITIES {T5HELM, T5SHIELD, T5SPELL, T5CLOAK, T5TOME, T5QUIVER, T5SCEPTER, T5SKULL, T5SEAL}
#define T6ABILITIES {T6HELM, T6SHIELD, T6SPELL, T6CLOAK, T6TOME, T6QUIVER, T6SCEPTER, T6SKULL, T6SEAL}
#define T7ABILITIES {T7HELM, T7SHIELD, T7SPELL, T7CLOAK, T7TOME, T7QUIVER, T7SCEPTER, T7SKULL, T7SEAL}
#define T8ABILITIES {T8HELM, T8SHIELD, T8SPELL, T8CLOAK, T8TOME, T8QUIVER, T8SCEPTER, T8SKULL, T8SEAL}

#define T0RING {T0DEFRING, T0ATTRING, T0SPDRING, T0WISRING, T0VITRING, T0DEXRING, T0HPRING, T0MPRING}
#define T1RING {T1DEFRING, T1ATTRING, T1SPDRING, T1WISRING, T1VITRING, T1DEXRING, T1HPRING, T1MPRING}
#define T2RING {T2DEFRING, T2ATTRING, T2SPDRING, T2WISRING, T2VITRING, T2DEXRING, T2HPRING, T2MPRING}
#define T3RING {T3DEFRING, T3ATTRING, T3SPDRING, T3WISRING, T3VITRING, T3DEXRING, T3HPRING, T3MPRING}
#define T4RING {T4DEFRING, T4ATTRING, T4SPDRING, T4WISRING, T4VITRING, T4DEXRING, T4HPRING, T4MPRING}

#define T0ARMOR {T0HEAVYARMOR, T0ROBE, T0LIGHTARMOR}
#define T1ARMOR {T1HEAVYARMOR, T1ROBE, T1LIGHTARMOR}
#define T2ARMOR {T2HEAVYARMOR, T2ROBE, T2LIGHTARMOR}
#define T3ARMOR {T3HEAVYARMOR, T3ROBE, T3LIGHTARMOR}
#define T4ARMOR {T4HEAVYARMOR, T4ROBE, T4LIGHTARMOR}
#define T5ARMOR {T5HEAVYARMOR, T5ROBE, T5LIGHTARMOR}
#define T6ARMOR {T6HEAVYARMOR, T6ROBE, T6LIGHTARMOR}
#define T7ARMOR {T7HEAVYARMOR, T7ROBE, T7LIGHTARMOR}
#define T8ARMOR {T8HEAVYARMOR, T8ROBE, T8LIGHTARMOR}
#define T9ARMOR {T9HEAVYARMOR, T9ROBE, T9LIGHTARMOR}
#define T10ARMOR {T10HEAVYARMOR, T10ROBE, T10LIGHTARMOR}
#define T11ARMOR {T11HEAVYARMOR, T11ROBE, T11LIGHTARMOR}
#define T12ARMOR {T12HEAVYARMOR, T12ROBE, T12LIGHTARMOR}
#define T13ARMOR {T13HEAVYARMOR, T13ROBE, T13LIGHTARMOR}
#define T14ARMOR {T14HEAVYARMOR, T14ROBE, T14LIGHTARMOR}

#define HPMPPOT {HPPOT, HPPOT, HPPOT, MPPOT}
#define MINORHPMPPOT {MINORHPPOT,MINORHPPOT,MINORHPPOT, MINORMPPOT}

#define BLADE LOFIOBJ, {4*8+4, 8*15, 1, 6}, 8,8,{0,10}

#define REDBOLT LOFIOBJ, {10*8, 8*9, 8, 8}, 10,10,{14,14}
#define WHITEBOLT LOFIOBJ, {10*8, 8*8, 8, 8}, 10,10,{14,14}
#define GREENBOLT LOFIOBJ, {10*8, 8*10, 8, 8}, 10,10,{14,14}
#define BLUEBOLT LOFIOBJ, {10*8, 8*11, 8, 8}, 10,10,{14,14}
#define PINKBOLT LOFIOBJ, {10*8, 8*12, 8, 8}, 10,10,{14,14}
#define BLACKBOLT LOFIOBJ2, {8*1,8*7,8,8}, 10,10,{14,14}
#define PURPLEBOLT LOFIOBJ2, {8*2,8*7,8,8}, 10,10,{14,14}
#define CHICKENBOLT LOFIOBJ2, {8*6,8*11,8,8}, 32,32,{4,4}
#define BLUESPIRITBOLT LOFICHAR, {8*8,8*26,8,8}, 32,32,{4,4}
#define GREYSPIRITBOLT LOFICHAR, {8*7,8*26,8,8}, 32,32,{4,4}

#define REDMAGIC LOFIOBJ, {4*8+3, 8*13+1, 3, 6}, 12,12,{0,8}
#define BLUEMAGIC LOFIOBJ, {4*8+3, 8*14, 3, 6}, 12,12,{0,8} 
#define GREENMAGIC LOFIOBJ, {12*8+3, 8*13+1, 3, 6}, 12,12,{0,8} 
#define PINKMAGIC LOFIOBJ, {12*8+3, 8*14, 3, 6}, 12,12,{0,8} 
#define ORANGEMAGIC LOFIOBJ2, {8*2+3, 8*10+1, 3, 6}, 12,12,{0,8}
#define PURPLEMAGIC LOFIOBJ5B, {14*8+2, 8*10+1, 3, 6}, 12,12,{0,8}
#define BLACKMAGIC LOFIOBJ2, {5*8+3, 8*10+1, 3, 6}, 12,12,{0,8}
#define YELLOWMAGIC LOFIOBJ2, {8*1+3, 8*10+1, 3, 6}, 12,12,{0,8}
#define CYANMAGIC LOFIOBJ2, {8*0+3, 8*10+1, 3, 6}, 12,12,{0,8}
#define DARKBLUEMAGIC LOFIOBJ2, {8*3+3, 8*10+1, 3, 6}, 12,12,{0,8}
#define CHRONUSMAGIC LOFIPROJS, {8*14+3, 8*2+1, 3, 6}, 12,12,{0,8}
#define BROWNMAGIC LOFIOBJ2, {4*8+3, 8*10+1, 3, 6}, 12,12,{0,8}

#define GREENARROW LOFIPROJS, {8, 8*7, 8, 8}, 8,8,{16,16}
#define REDARROW LOFIPROJS, {8*2, 8*7, 8, 8}, 8,8,{16,16}
#define BLUEARROW LOFIPROJS, {8*3, 8*7, 8, 8}, 8,8,{16,16}
#define GOLDARROW LOFIPROJS, {8*4, 8*7, 8, 8}, 8,8,{16,16}
#define LIGHTBOLT LOFIOBJ, {8*10+3,8*5+1,3,7}, 8,8,{0,10}
#define HEAVYBOLT LOFIOBJ, {8*10+3,8*5+1,3,7}, 8,8,{0,10}
#define YELLOWARROW LOFIOBJ5B, {8*13, 8*11, 8, 8}, 8,8,{16,16}
#define BLACKARROW LOFIOBJ5B, {8*14, 8*11, 8, 8}, 8,8,{16,16}


#define REDSTAR LOFIOBJ, {8*0,8*6,8,8}, 10, 10, {14,14} // bleeding
#define WHITESTAR LOFIOBJ, {8*6,8*6,8,8}, 10, 10, {14,14} // ignore def
#define BLUESTAR LOFIOBJ, {8*3,8*6,8,8}, 10, 10, {14,14} // confuse
#define GREENSTAR LOFIOBJ, {8*3,8*7,8,8}, 10, 10, {14,14} // quiet
#define PURPLESTAR LOFIOBJ, {8*0,8*7,8,8}, 10, 10, {14,14} // slowed
#define YELLOWSTAR LOFIOBJ, {8*6,8*7,8,8}, 10, 10, {14,14} // blind
#define ORANGESTAR LOFIOBJ, {8*9,8*7,8,8}, 10, 10, {14,14} // stunned
#define BLACKSTAR LOFIOBJ,  {8*10,8*7,8,8}, 10, 10, {14,14} // paralyzed

#define REDCIRCLEMAGIC LOFIOBJ, {8*1,8*9,8,8}, 32,32,{4,4}
#define ARCMAGIC LOFIOBJ3, {8*1,8*22,8,8}, 32,32,{4,4}
#define ORYXCIRCLE LOFIOBJ2, {8*0,8*4,8,8}, 32,32,{4,4}
#define ORYXTEAR LOFIOBJ2, {8*1, 8*4, 8,8}, 10, 10, {14,14}
#define ORYXARROW LOFIOBJ3, {8*6, 8*47, 8,8}, 10, 10, {14,14}
#define REDSHARD LOFIOBJ4, {8*1, 8*12,8,8}, 10,10,{14,14}
#define DJINNSHOT LOFIOBJ2, {8*9,8*4,8,8}, 20,20,{10,10}
#define BLUEBOOMERANG LOFIOBJ, {8*3, 8*11,8,8}, 32, 32, {4,4}
#define SHILEDBULLET LOFIOBJ2, {8*3, 8*1, 8, 8}, 32, 32, {8,8}
#define FIREBALL LOFICHAR2, {8*8, 8*3, 8, 8}, 32, 32, {4,4}
#define LONGFIREBOLT LOFICHAR2, {8*10, 8*2, 8, 16}, 32, 32, {4,24}
#define ZSHOT LOFIOBJ4, {8*0, 8*12, 8, 8}, 32, 32, {4,4} 
#define WHITECROSS LOFIOBJ, {8*8,8*8,8,8}, 32,32,{4,4}

#define WHITEMISSILE LOFIOBJ2, {12*8+2, 9*8+1, 5,5}, 8,8,{12,12}
#define REDMISSILE LOFIOBJ2, {8*10+2, 8*8+1, 5,5}, 8,8,{12,12}
#define BLACKMISSILE LOFIOBJ2, {8*10+2, 8*9+1, 5,5}, 8,8,{12,12}
#define GREENMISSILE LOFIOBJ, {13*8+2, 13*8+1, 5,5}, 8,8,{12,12}
#define YELLOWMISSILE LOFIOBJ2, {11*8+2, 9*8+1, 5,5}, 8,8,{12,12}
#define PURPLEMISSILE LOFIOBJ2, {8*11+2, 8*8+1, 5,5}, 8,8,{12,12}
#define BLUEMISSILE LOFIOBJ, {8*5+2, 8*14+1, 5,5}, 8,8,{12,12}
#define AQUAMISSILE LOFIOBJ2, {12*8+2, 8*8+1, 5,5}, 8,8,{12,12}
#define ORANGEMISSILE LOFIOBJ2, {4*8+2, 8*7+1, 5,5}, 8,8,{12,12}
#define CULTMISSILE LOFIPROJS, {8*15, 8*3, 8,8}, 8,8,{16,16}

#define NULL_PEC 0, 0, 0, 0, 0, 0.0f // pentaract tower uses this. fake PEC 

// in order of how they're written its: unused, orbits, rotates, oscillates, ignoredef, inflictSE, diagonal, piercing
#define BITS_NONE 0b00000000
#define BITS_DIAGONAL 0b00000010 
#define BITS_ROTATE_DIAGONAL 0b00100010
#define BITS_DIAGONAL_PIERCE 0b00000011
#define BITS_PIERCING 0b00000001
#define BITS_OSCILLATING_DIAGONAL 0b00010010
#define BITS_ROTATING 0b00100000
#define BITS_IGNOREDEF 0b00001000
#define BUTS_IGNOREDEF_ROTATE 0b00101000
#define BITS_ROTATE_INFLICTSE 0b00100100
#define BITS_OSCILLATING 0b00010000
#define BITS_INFLICTSE 0b00000100



/*
these maps, tables, vectors are used to access data associated with an enum
basically, this is a database.........
*/

struct projectilePPD{
    textureEnums texture;
    SDL_Rect rect;
    int boxWidth;
    int boxHeight;
    glm::vec2 boxOffset;
};

struct abilityData{
    int cooldown;
    int mprequired;
};

struct tomeData{
    int hp;
};

struct BaseStatData{
    unsigned short hp;
    unsigned short mp;
    unsigned char attack;
    unsigned char defense;
    unsigned char speed;
    unsigned char dexterity;
    unsigned char vitality;
    unsigned char wisdom;

    inline bool itemProvidesNoStats(){
        return hp == 0 && mp == 0 && attack == 0 && defense == 0 && speed == 0 && dexterity == 0 && vitality == 0 && wisdom == 0;
    }
};

struct startingEquipment{
    items startingWeapon;
    items startingEquipment;
};

struct spritedata { // can be used to initialize SpriteComponents.
    textureEnums assetId; 
    int width;
    int height;
    SDL_Rect srcRect;
    int zIndex;
    bool isFixed;
    bool diagonalSprite;
};

struct playerPECupdateData{
    std::bitset<8> PECbools;

    unsigned short duration;
    unsigned short projectileSpeed;
    unsigned short minDamage;
    unsigned short maxDamage;
    unsigned char shots;
    float arcgap;

    // SPRITE INFO FOR PROJECTILE!! NOT THE ITEM ITSELF!!!!
    textureEnums spriteasetId;
    SDL_Rect spriteSrcRect;

    unsigned short boxdwith;
    unsigned short boxheight;
    glm::vec2 boxoffset;

    // bool oscillation;
    float amplitude = 0.0f;
    float frequency = 0.0f;
};

struct wallData{
    textureEnums texture;
    std::vector<glm::ivec2> walls;
    glm::ivec2 alpha; //coordinates of blank sprite
};

struct AnimatedShootingData {
    unsigned char extraPixels;
};


struct enemyPECData{ // same as PEC minus parent, lastEmissionTime, velocity, minDamage
    std::bitset<8> PECbools;

    unsigned short repeatFrequency; // ms per shot 
    unsigned short projectileSpeed; 
    unsigned short duration; // ms until death of projectile
    unsigned short maxDamage; // max damage. monsters only use one damage
    unsigned char shots = 0; // dont use odd number of shots with 360, 720, etc
    float arcgap = 0; // sum of all angles between shots

    textureEnums spriteassetId; 
    SDL_Rect spritesrcRect;
    unsigned short boxwidth; // box info for projectile
    unsigned short boxheight;
    glm::vec2 boxoffset; 

    statuses statusEffect = NULL_STATUS_EFFECT;
    unsigned short durationMS = 0;

    float amplitude = 0.0f;
    float frequency = 0.0f;
};

struct statData{ // used to populate HPMPComponent, OffenseStatComponent, SpeedStatComponent, ClassNameComponent for MONSTERS
    float activehp;
    float activemp;
    unsigned char activeattack;
    unsigned char activedefense;
    unsigned char activespeed;
    unsigned char activedexterity;
    unsigned char activevitality;
    unsigned char activewisdom;
    soundEnums hitsound;
    soundEnums deathsound;
};


struct boxColliderData { // can be used to initialize boxcomponents
    unsigned short width;
    unsigned short height;
    glm::vec2 offset;
};


// can be used for ChaseAIComponent and AnimatedChaseAIComponent (unless in the future their fields vary)
// and the neutrals, why not 
struct aiChaseData {
    unsigned short detectRange; // distance in pixels where monster can detect player (ex: path to player)
    unsigned short engageRange; // distance where monster will further engage if possible (ex: shoot)
    unsigned short maxDistance; // closest distance in pixels monster wants to get to player (so monster doens't stand on top of player, or if so they do)

};

struct trapaidata {
    unsigned short engageRange;
    unsigned char iginitionFrame; 
};


struct animationData{
    int numFrames;
    double frameSpeedRate;
    int xmin;

};

//quiver data only pertains to the projectile which is emitted!
struct quiverData{
    int minDamage;
    int maxDamage;
    textureEnums texture;
    SDL_Rect srcRect;
    statuses debuff;
};

struct ItemTableComponentData{
    std::multimap<int, std::vector<items>> dropTable;
};

struct helmData{
    Uint32 duration;
};

struct scepterData{
    int maxNumberTargets;
    int damage;
    int DamageReductionPerTarget;
};

struct skullData{
    int damage;
    float radius;
};

struct spellData{
    int minDamage;
    int maxDamage;
    textureEnums texture;
    SDL_Rect srcRect;
};

struct shieldData{
    statuses debuff;
    int minDamage;
    int maxDamage;
    int debuffDuration;
    int numshots;
};

struct minionSpawnerData{
    std::vector<sprites> minions;
    unsigned char maxMinions;
    Uint32 respawnInterval;
    bool spawnOnlyOnce;
};

struct travelDistanceData{
    float distance;
    float distanceModMax;
};

struct secondaryPECdata{
    unsigned int RFmin;
    float RFmaxmod;
};

extern std::unordered_map<items, skullData> itemToSkullData;
extern std::unordered_map<items, scepterData> itemToScepterData;
extern std::vector<sprites> chickens;
extern std::unordered_set<sprites> hasDeathAction;
extern std::unordered_set<sprites> towers;
extern std::unordered_map<sprites, travelDistanceData> spriteToMinionTravelDistanceData;
extern std::unordered_map<sprites, minionSpawnerData> spriteToMinionSpawnerData;
extern std::unordered_map<sprites, monsterSubGroups> spriteToMonsterSubGroups;
extern std::vector<sprites> eventBosses;
extern std::vector<sprites> gods;
extern std::unordered_map<sprites, secondaryPECdata> spriteToSPECRepeatFreq;
extern std::unordered_map<items, shieldData> itemEnumToShieldData;
extern std::unordered_map<items, spellData> itemEnumToSpellData;
extern std::vector<std::vector<int>> gordonLairOnlyFloors;
extern std::vector<std::vector<int>> gordonLair;
extern std::unordered_map<int, SDL_Color> miniMapColors;
extern std::unordered_map<wallTheme, textureEnums> wallThemeToPortalUITexture;
extern std::vector<std::string> defaultNames;
extern std::vector<glm::vec2> vaultSpawns;
extern std::unordered_map<wallTheme, glm::vec2> wallThemeToSpawnPoint;
extern std::unordered_map<wallTheme, std::vector<std::vector<roomSpawn>>> wallThemeToMonsterSpawns;
extern std::unordered_map<wallTheme, spritedata> wallThemeToSpriteData;
extern std::unordered_map<std::string, textureEnums> PortalTitleToTexture; 
extern std::unordered_map<wallTheme, glm::ivec2> wallThemeToFloor;
extern std::unordered_map<sprites, std::string> spriteToName;
extern std::vector<std::vector<int>> nexusMap;
extern std::vector<std::vector<int>> vaultMap;
extern std::vector<std::vector<int>> nexusMapOnlyFloors;
extern std::vector<std::vector<int>> vaultMapOnlyFloors;
extern std::unordered_map<items, const char *> abillityItemToInfo;
extern std::unordered_map<items, const char *> consumableItemToInfo;
extern std::unordered_map<items, const char *> itemToDescription;
extern std::unordered_map<items, const char *> itemToName;
extern std::unordered_map<items, textureEnums> itemToIconTexture;
extern std::unordered_map<classes, startingEquipment> classesToStartingItems;
extern std::unordered_map<classes, const char*> classesToString;
extern std::unordered_map<items, helmData> itemEnumToHelmData;
extern std::unordered_map<sprites, ItemTableComponentData> spriteEnumToItemTableComponentData;
extern std::unordered_map<items, sprites> itemEnumToLootBagSpriteEnum;
extern std::unordered_map<items, abilityData> itemEnumToAbilityData;
extern std::unordered_map<items, quiverData> itemEnumToQuiverData;
extern std::unordered_map<items, tomeData> itemEnumToTomeData;
extern std::unordered_map<classes, BaseStatData> classToBaseStats;
extern std::unordered_map<items, BaseStatData> itemEnumToStatData;
extern std::vector<groups> validWeapons;
extern std::vector<groups> validability;
extern std::vector<groups> validarmor;
extern std::unordered_map<sprites, enemyCategory> spriteToEnemyCategory;
extern std::unordered_map<items, groups> itemToGroup;
extern std::unordered_map<items, spritedata> itemEnumTospriteData;
extern std::unordered_map<items, playerPECupdateData> itemEnumToPECdata;
extern std::vector<long> nextXPToLevelUp;
extern std::unordered_map<wallTheme, wallData> wallThemeToWallData;
extern std::unordered_map<classes, std::array<int, 8>> maxStats;
extern std::unordered_map<classes, AnimatedShootingData> classToAnimatedShootingData;
extern std::unordered_map<sprites, AnimatedShootingData> spriteToAnimatedShootingData;
extern std::unordered_map<sprites, enemyPECData> spriteEnumToPEC;
extern std::unordered_map<sprites, statData> spriteEnumToStatData;
extern std::unordered_map<boxColliders, boxColliderData> bcEnumToData;
extern std::unordered_map<sprites, spritedata> enumToSpriteComponent;
extern std::unordered_map<sprites, boxColliders> spriteToBC;
extern std::unordered_map<sprites, aiChaseData> spritesToaiChaseData;
extern std::unordered_map<sprites, trapaidata> spritetotrap;
extern std::unordered_map<sprites, animationData> spriteToAnimationData;

#endif