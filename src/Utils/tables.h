#pragma once 

#include <unordered_map>
#include <array>
#include "enums.h"
#include <vector>
#include "../../libs/glm/glm.hpp"
#include "../../libs/SDL2/SDL.h"
#include <memory>
#include <map>

/*
these maps, tables, vectors are used to access data associated with an enum
basically, this is a database.........
*/

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
    unsigned short duration;
    unsigned short minDamage;
    unsigned short maxDamage;
    unsigned short projectileSpeed;
    bool piercing; 
    unsigned char shots;
    float arcgap;

    // SPRITE INFO FOR PROJECTILE!! NOT THE ITEM ITSELF!!!!
    textureEnums spriteasetId;
    unsigned char spriteWidth;
    unsigned char spriteHeight;
    SDL_Rect spriteSrcRect;
    int zIndex;
    bool spritediagonalsprite;

    unsigned short boxdwith;
    unsigned short boxheight;
    glm::vec2 boxoffset;
};

struct wallData{
    textureEnums texture;
    std::vector<glm::ivec2> walls;
    glm::ivec2 alpha; //coordinates of blank sprite
};

struct AnimatedShootingData {
    unsigned char extraPixels;
};


struct enemyPECData{ // same as PEC minus parent, lastEmissionTime, velocity
    bool isShooting; 
    unsigned short repeatFrequency; // ms per shot 
    unsigned short duration; // ms until death of projectile
    unsigned short damage; 
    unsigned short projectileSpeed;
    bool piercing;
    unsigned char shots; // dont use odd number of shots with 360, 720, etc
    float arcgap; 

    textureEnums spriteassetId; // sprite info for projectile
    unsigned char spritewidth;
    unsigned char spriteheight;
    SDL_Rect spritesrcRect;
    unsigned char spritezIndex;
    bool spriteisFixed; // fixed sprites stay in same spot on screen 
    bool spritediagonalSprite;

    unsigned short boxwidth; // box info for projectile
    unsigned short boxheight;
    glm::vec2 boxoffset; 
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
    std::map<int, std::vector<items>> dropTable;
};

struct helmData{
    Uint32 duration;
};

extern std::unordered_map<items, helmData> itemEnumToHelmData;

extern std::unordered_map<sprites, ItemTableComponentData> spriteEnumToItemTableComponentData;

extern std::unordered_map<items, sprites> itemEnumToLootBagSpriteEnum;

extern std::unordered_map<items, abilityData> itemEnumToAbilityData;

extern std::unordered_map<items, quiverData> itemEnumToQuiverData;

extern std::unordered_map<items, tomeData> itemEnumToTomeData;

extern std::unordered_map<classes, BaseStatData> classToBaseStats;

extern std::unordered_map<items, BaseStatData> itemEnumToStatData;

// indexing with player classname returns valid item group for weapon slot
extern std::vector<groups> validWeapons;
extern std::vector<groups> validability;
extern std::vector<groups> validarmor;

extern std::unordered_map<sprites, enemyCategory> spriteToEnemyCategory;


extern std::unordered_map<items, groups> itemToGroup;

// DATA FOR ITEM SPRITE. NOT FOR WEAPON PROJCETILES! SEE itemEnumToPEC for that
extern std::unordered_map<items, spritedata> itemEnumTospriteData;


extern std::unordered_map<items, playerPECupdateData> itemEnumToPECdata;

extern std::vector<long> nextXPToLevelUp;

extern std::unordered_map<wallTheme, wallData> wallThemeToWallData;

extern std::unordered_map<classes, std::array<int, 8>> maxStats;

extern std::unordered_map<classes, AnimatedShootingData> classToAnimatedShootingData;

extern std::unordered_map<sprites, AnimatedShootingData> spriteToAnimatedShootingData;


// GIVEN A MONSTER SPRITE, RETURNS PROJECTILE EMITTER COMPONENT
extern std::unordered_map<sprites, enemyPECData> spriteEnumToPEC;


// GIVEN SPRITE ENUM FOR A MONSTER, RETURNS ASSOCIATED STAT DATA and the sounds
extern std::unordered_map<sprites, statData> spriteEnumToStatData;

// GIVEN BOXCOLLIDER ENUM RETURNS ASSOCIATED BOXCOLLIDER DATA
// copy and paste to spriteEnumToPEC
extern std::unordered_map<boxColliders, boxColliderData> bcEnumToData;

// GIVEN A SPRITE ENUM, RETURNS THE ASSOCIATE SPRITE COMPONENT DATA
extern std::unordered_map<sprites, spritedata> enumToSpriteComponent;

extern std::unordered_map<classes, spritedata> classToSpriteData;

extern std::unordered_map<sprites, boxColliders> spriteToBC;


extern std::unordered_map<sprites, aiChaseData> spritesToaiChaseData;


extern std::unordered_map<sprites, trapaidata> spritetotrap;

extern std::unordered_map<sprites, animationData> spriteToAnimationData;