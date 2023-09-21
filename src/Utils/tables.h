#pragma once

#include <unordered_map>
#include "enums.h"
#include <vector>
#include "../../libs/glm/glm.hpp"
#include "../../libs/SDL2/SDL.h"
#include <memory>

/*
these maps, tables, vectors are used to access data associated with an enum
*/

// indexing with player classname returns valid item group for weapon slot
std::vector<groups> validWeapons = {SWORD,BOW,SWORD,WAND,SWORD,SWORD,SWORD,SWORD,SWORD,SWORD,SWORD,SWORD,SWORD};

std::unordered_map<sprites, enemyCategory> spriteToEnemyCategory = {
    {SKELETON0, ASC},
    {SKELETON1, ASC},
    {SKELETON2, ASC},
    {SKELETON3, AS},
    {SKELETON4, ASC},
    {REDKNIGHT0, SC},
    {SHATTERSBOMB, T}
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

std::unordered_map<items, groups> itemToGroup = {
    {T0SWORD, SWORD},
    {T1SWORD, SWORD},
    {T2SWORD, SWORD},
    {T3SWORD, SWORD},
    {T4SWORD, SWORD},
    {T5SWORD, SWORD},
    {T6SWORD, SWORD},
    {T7SWORD, SWORD},
    {T8SWORD, SWORD},
    {T9SWORD, SWORD},
    {T10SWORD, SWORD},
    {T11SWORD, SWORD},
    {T12SWORD, SWORD},
    {T13SWORD, SWORD},
    {T14SWORD, SWORD},
    {IMPBLADE, SWORD},
    {T0BOW, BOW},
    {T1BOW, BOW},
    {T2BOW, BOW},
    {T3BOW, BOW},
    {T4BOW, BOW},
    {T5BOW, BOW},
    {T6BOW, BOW},
    {T7BOW, BOW},
    {T8BOW, BOW},
    {T9BOW, BOW},
    {T10BOW, BOW},
    {T11BOW, BOW},
    {T12BOW, BOW},
    {T13BOW, BOW},
    {T14BOW, BOW},
    {T0WAND, WAND},
    {T1WAND, WAND},
    {T2WAND, WAND},
    {T3WAND, WAND},
    {T4WAND, WAND},
    {T5WAND, WAND},
    {T6WAND, WAND},
    {T7WAND, WAND},
    {T8WAND, WAND},
    {T9WAND, WAND},
    {T10WAND, WAND},
    {T11WAND, WAND},
    {T12WAND, WAND},
    {T13WAND, WAND},
    {T14WAND, WAND}
    
};

// DATA FOR ITEM SPRITE. NOT FOR WEAPON PROJCETILES! SEE itemEnumToPEC for that
std::unordered_map<items, spritedata> itemEnumTospriteData = {
    {T0SWORD, {LOFIOBJ5, 8, 8, {8*0, 8*3, 8, 8}, 12, true, false}},
    {T1SWORD, {LOFIOBJ5, 8, 8, {8*1, 8*3, 8, 8}, 12, true, false}},
    {T2SWORD, {LOFIOBJ5, 8, 8, {8*2, 8*3, 8, 8}, 12, true, false}},
    {T3SWORD, {LOFIOBJ5, 8, 8, {8*3, 8*3, 8, 8}, 12, true, false}},
    {T4SWORD, {LOFIOBJ5, 8, 8, {8*4, 8*3, 8, 8}, 12, true, false}},
    {T5SWORD, {LOFIOBJ5, 8, 8, {8*5, 8*3, 8, 8}, 12, true, false}},
    {T6SWORD, {LOFIOBJ5, 8, 8, {8*6, 8*3, 8, 8}, 12, true, false}},
    {T7SWORD, {LOFIOBJ5, 8, 8, {8*7, 8*3, 8, 8}, 12, true, false}},
    {T8SWORD, {LOFIOBJ5, 8, 8, {8*8, 8*3, 8, 8}, 12, true, false}},
    {T9SWORD, {LOFIOBJ5, 8, 8, {8*9, 8*3, 8, 8}, 12, true, false}},
    {T10SWORD, {LOFIOBJ5, 8, 8, {8*10, 8*3, 8, 8}, 12, true, false}},
    {T11SWORD, {LOFIOBJ5, 8, 8, {8*11, 8*3, 8, 8}, 12, true, false}},
    {T12SWORD, {LOFIOBJ5, 8, 8, {8*12, 8*3, 8, 8}, 12, true, false}},
    {T13SWORD, {LOFIOBJ5, 8, 8, {8*13, 8*3, 8, 8}, 12, true, false}},
    {T14SWORD, {LOFIOBJ5, 8, 8, {8*14, 8*3, 8, 8}, 12, true, false}},
    {IMPBLADE, {LOFIOBJ2, 8, 8, {8*3, 8*11, 8, 8}, 12, true, false}}
};

// contains fields-to-be updated upon equipping a new weapon (sprite, speed, duration)
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

std::unordered_map<items, playerPECupdateData> itemEnumToPECdata = {
    {T0SWORD, {350,45,90,640,false,1,0,LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false,8,8,{0,10}}},
    {T1SWORD,{350,60,105,640,false,1,0,LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false,8,8,{0,10}}},
    {T2SWORD,{350,75,105,640,false,1,0,LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false,8,8,{0,10}}},
    {T3SWORD,{350,75,120,640,false,1,0,LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false,8,8,{0,10}}},
    {T4SWORD,{350,75,135,640,false,1,0,LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false,8,8,{0,10}}},
    {T5SWORD,{350,90,135,640,false,1,0,LOFIOBJ, 8, 8, {10*8, 8*9, 8, 8}, 3, true,10,10,{14,14}}},
    {T6SWORD,{350,30,210,640,false,1,0,LOFIOBJ, 8, 8, {10*8, 8*8, 8, 8}, 3, true,10,10,{14,14}}},
    {T7SWORD,{350,150,180,640,false,1,0,LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false,8,8,{0,10}}},
    {T8SWORD,{350,180,255,640,false,1,0,LOFIOBJ2,8, 8, {8*1,8*7,8,8}, 3, true, 8, 8, {0,10}}},
    {T9SWORD,{350,195,255,640,false,1,0,LOFIOBJ, 8, 8, {10*8, 8*8, 8, 8}, 3, true,10,10,{14,14}}},
    {T10SWORD,{350,210,255,640,false,1,0,LOFIOBJ, 8, 8, {10*8, 8*9, 8, 8}, 3, true,10,10,{14,14}}},
    {T11SWORD,{350,210,270,640,false,1,0,LOFIOBJ, 8, 8, {10*8, 8*11, 8, 8}, 3,true,10,10,{14,14}}},
    {T12SWORD,{350,220,275,640,false,1,0,LOFIOBJ2, 8, 8, {2*8, 8*7, 8, 8}, 3,true,10,10,{14,14}}},
    {T13SWORD,{350,235,290,640,false,1,0,LOFIOBJ, 8, 8, {10*8, 8*10, 8, 8}, 3, true,10,10,{14,14}}},
    {T14SWORD,{350,250,305,640,false,1,0,LOFIOBJ2,8, 8, {8*1,8*7,8,8}, 3, true, 10, 10, {14,14}}},
    {IMPBLADE,{350,90,135,500,false,2,40,LOFIOBJ, 8, 8, {10*8, 8*9, 8, 8}, 3, true,10,10,{14,14}}},

    {T0BOW, {440, 10, 40, 1024, true,1,0,LOFIPROJS, 8, 8, {8, 8*7, 8, 8}, 3,true,8,8,{16,16}}},
    {T1BOW, {440, 15, 45, 1024, true,1,0,LOFIPROJS, 8, 8, {8, 8*7, 8, 8}, 3,true,8,8,{16,16}}},
    {T2BOW, {469, 20, 50, 1024, true,1,0, LOFIOBJ, 3,6, {8*10,8*5,3,6},3,false,8,8,{0,10}}},
    {T3BOW, {440, 25, 55, 1024, true,1,0,LOFIPROJS, 8, 8, {8, 8*7, 8, 8}, 3,true,8,8,{16,16}}},
    {T4BOW, {440, 35, 65, 1024, true,1,0,LOFIPROJS, 8, 8, {8, 8*7, 8, 8}, 3,true,8,8,{16,16}}},
    {T5BOW, {440, 45, 65, 1024, true,1,0,LOFIOBJ, 3, 6, {4*8+3, 8*13+1, 3, 6}, 3, true,12,12,{0,8}}},
    {T6BOW, {440, 30, 50, 1024, true,2,11.25,LOFIOBJ, 3,6, {8*10,8*5,3,6},3,true,8,8,{0,10}}},
    {T7BOW, {469, 50, 70, 1024, true,1,0,LOFIOBJ, 3,6, {8*10,8*5,3,6},3,false,8,8,{0,10}}},
    {T8BOW, {440, 40, 60, 1024, true,3,8,LOFIPROJS, 8, 8, {8*4, 8*7, 8, 8}, 3,true,8,8,{16,16}}},
    {T9BOW, {440, 40, 65, 1024, true,3,8,LOFIPROJS, 8, 8, {8, 8*7, 8, 8}, 3,true,8,8,{16,16}}},
    {T10BOW, {440, 45, 65, 1024, true,3,8,LOFIPROJS, 8, 8, {8*3, 8*7, 8, 8}, 3,true,8,8,{16,16}}},
    {T11BOW, {440, 45, 70, 1024, true,3,8,LOFIPROJS, 8, 8, {8*2, 8*7, 8, 8}, 3,true,8,8,{16,16}}},
    {T12BOW, {440, 50, 70, 1024, true,3,8,LOFIPROJS, 8, 8, {8*3, 8*7, 8, 8}, 3,true,8,8,{16,16}}},
    {T13BOW, {440, 55, 75, 1024, true,3,8,LOFIPROJS, 8, 8, {8*13, 8*11, 8, 8}, 3,true,8,8,{16,16}}},
    {T14BOW, {440, 60, 80, 1024, true,3,8,LOFIOBJ5B, 8, 8, {8*14, 8*11, 8, 8}, 3,true,8,8,{16,16}}},

    {T0WAND, {1298, 20, 40, 640, false, 1, 0,LOFIOBJ, 3, 6, {4*8+3, 8*13+1, 3, 6}, 3, false,12,12,{0,8}}},
    {T1WAND, {1298, 30, 50, 640, false, 1, 0,LOFIOBJ, 3, 6, {4*8+3, 8*14, 3, 6}, 3, false,12,12,{0,8}}},
    {T2WAND, {1298, 35, 55, 640, false, 1, 0,LOFIOBJ, 3, 6, {4*8+3, 8*14, 3, 6}, 3, false,12,12,{0,8}}},
    {T3WAND, {1298, 40, 60, 640, false, 1, 0,LOFIOBJ, 3, 6, {4*8+3, 8*14, 3, 6}, 3, false,12,12,{0,8}}},
    {T4WAND, {1298, 50, 70, 640, false, 1, 0,LOFIOBJ, 3, 6, {4*8+3, 8*14, 3, 6}, 3, false,12,12,{0,8}}},
    {T5WAND, {1298, 60, 80, 640, false, 1, 0,LOFIOBJ, 3, 6, {12*8+3, 8*14, 3, 6}, 3, false,12,12,{0,8}}},
    {T6WAND, {1298, 70, 90, 640, false, 1, 0,LOFIOBJ, 8, 8, {10*8, 8*12, 8, 8}, 3,false,10,10,{14,14}}},
    {T7WAND, {1298, 80, 100, 640, false, 1, 0,LOFIOBJ, 3, 6, {4*8+3, 8*13+1, 3, 6}, 3, false,12,12,{0,8}}},
    {T8WAND, {1298, 80, 120, 640, false, 1, 0,LOFIOBJ, 3, 6, {12*8+3, 8*13+1, 3, 6}, 3, false,10,10,{14,14}}},
    {T9WAND, {1298, 85, 125, 640, false, 1, 0,LOFIOBJ, 3, 6, {12*8+3, 8*14, 3, 6}, 3, false,12,12,{0,8}}},
    {T10WAND, {1298, 85, 130, 640, false, 1, 0,LOFIOBJ, 3, 6, {4*8+3, 8*13+1, 3, 6}, 3, false,12,12,{0,8}}},
    {T11WAND, {1298, 90, 130, 640, false, 1, 0,LOFIOBJ, 3, 6, {12*8+3, 8*13+1, 3, 6}, 3, false,10,10,{14,14}}},
    {T12WAND, {1298, 90, 135, 640, false, 1, 0,LOFIOBJ, 3, 6, {4*8+3, 8*14, 3, 6}, 3, false,12,12,{0,8}}},
    {T13WAND, {1298, 100, 135, 640, false, 1, 0,LOFIPROJS, 3, 6, {4*13+3, 8*10, 3, 6}, 3, false,12,12,{0,8}}},
    {T14WAND, {1298, 105, 140, 640, false, 1, 0,LOFIPROJS, 3, 6, {4*14+3, 8*10, 3, 6}, 3, false,12,12,{0,8}}},

};

std::vector<long> nextXPToLevelUp{0,50,200,450,800,1250,1800,2450,3200,4050,5000,6050,7200,8450,9800,11250,12800,14450,16200,18050};

struct wallData{
    textureEnums texture;
    std::vector<glm::ivec2> walls;
    glm::ivec2 alpha; //coordinates of blank sprite
};

std::unordered_map<wallTheme, wallData> wallThemeToWallData = {
    {UDL, {LOFIENVIRONMENT, {glm::ivec2(0,0), glm::ivec2(0,1), glm::ivec2(0,2), glm::ivec2(0,3), glm::ivec2(0,4)}, glm::ivec2(14,0)}}
};

std::unordered_map<classes, std::array<int, 8>> maxStats = {
    {ARCHER, {700,252,75,25,50,50,75,50}},
    {PRIEST, {670,385,50,25,50,50,75,75}},
    {WARRIOR, {770,252,75,25,50,50,75,50}}
};

struct AnimatedShootingData {
    unsigned char extraPixels;
};

std::unordered_map<classes, AnimatedShootingData> classToAnimatedShootingData = {
    {WARRIOR, {4}},
    {ARCHER, {0}},
    {PRIEST, {3}}
};

std::unordered_map<sprites, AnimatedShootingData> spriteToAnimatedShootingData = {
    {SKELETON0, {0}},
    {SKELETON1, {4}},
    {SKELETON2, {4}},
    {SKELETON3, {0}},
    {SKELETON4, {0}}
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

// GIVEN A MONSTER SPRITE, RETURNS PROJECTILE EMITTER COMPONENT
std::unordered_map<sprites, enemyPECData> spriteEnumToPEC = {
    {SKELETON0, {false, 500, 400, 5, 500, false, 1, 0, LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false, false, 8,8,{0,10}}},
    {SKELETON1, {false, 1000, 400, 10, 500, false, 1, 0, LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false, false, 8,8,{0,10}}},
    {SKELETON2, {false, 1000, 400, 15, 500, false, 1, 0, LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false, false, 8,8,{0,10}}},
    {SKELETON3, {false, 1000, 1000, 20, 750, false, 3, 8, LOFIOBJ, 8, 8, {10*8, 8*12, 8, 8}, 3,false, true, 10,10,{14,14}}},
    {SKELETON4, {false, 150, 500, 25, 500, false, 1, 0, LOFIOBJ, 8, 8, {10*8, 8*9, 8, 8}, 3,false, true,10,10,{14,14}}},
    {REDKNIGHT0, {false, 300, 200, 50, 500, false, 1, 0, LOFIOBJ,8,8,{8*1,8*9,8,8},3,false,false, 32,32,{4,4}}},
    {SHATTERSBOMB, {false, 0,250,15,750,false,24,360,LOFIOBJ4,8,8,{8*1, 8*12,8,8},3,false,true,10,10,{14,14}}}
    //spriteEnum                             //boxCollEnum, PEC sprite comp
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

// GIVEN SPRITE ENUM FOR A MONSTER, RETURNS ASSOCIATED STAT DATA and the sounds
std::unordered_map<sprites, statData> spriteEnumToStatData = {
                // currently, loadmonster() doesn't give rigidybodycomp if spd is set to 0
                //hp,mp,att,def,spd,dex,vit,wis
    {SKELETON0, {100,1,1,5,20,1,10,1,SKELETONSHIT,SKELETONSDEATH}},
    {SKELETON1, {100,1,1,10,20,1,10,1,SKELETONSHIT,SKELETONSDEATH}},
    {SKELETON2, {100,1,1,15,20,1,10,1,SKELETONSHIT,SKELETONSDEATH}},
    {SKELETON3, {100,1,1,5,0,1,10,1,SKELETONSHIT,SKELETONSDEATH}}, //mage
    {SKELETON4, {100,1,1,15,35,1,10,1,SKELETONSHIT,SKELETONSDEATH}}, //king
    {REDKNIGHT0, {1000,1,1,30,20,1,10,1, REDKNIGHTHIT, REDKNIGHTDEATH}},
    {SHATTERSBOMB, {0,0,0,0,0,0,0,0,WOODENWALLSHIT,WOODENWALLSDEATH}}
};

struct boxColliderData { // can be used to initialize boxcomponents
    unsigned short width;
    unsigned short height;
    glm::vec2 offset;
};

// GIVEN BOXCOLLIDER ENUM RETURNS ASSOCIATED BOXCOLLIDER DATA
// copy and paste to spriteEnumToPEC
std::unordered_map<boxColliders, boxColliderData> bcEnumToData = {
    {STANDARD, {38,18,{6,30}}},
    {MAGIC, {12,12,{0,8}}},
    {BOLT, {10,10,{14,14}}},
    {BLADE, {8,8,{0,10}}},
    {ARROW, {8,8,{16,16}}},
    {CIRCLEMAGIC, {32,32,{4,4}}},
    {SHATTERSBOMBBOX, {32,12,{8,36}}},
    {LOOTBAG, {8*5,8*5,{0,0}}}

};

// GIVEN A SPRITE ENUM, RETURNS THE ASSOCIATE SPRITE COMPONENT DATA
std::unordered_map<sprites, spritedata> enumToSpriteComponent = {
    {REDMAGIC,{LOFIOBJ, 3, 6, {4*8+3, 8*13+1, 3, 6}, 3, false, false}},
    {BLUEMAGIC,{LOFIOBJ, 3, 6, {4*8+3, 8*14, 3, 6}, 3, false, false}},
    {GREENMAGIC,{LOFIOBJ, 3, 6, {12*8+3, 8*13+1, 3, 6}, 3, false, false}},
    {PURPLEMAGIC,{LOFIOBJ, 3, 6, {12*8+3, 8*14, 3, 6}, 3,false, false}},
    {DEFAULTBLADE,{LOFIOBJ, 1, 6, {4*8+4, 8*15, 1, 6}, 3, false, false}},
    {WHITEBOLT,{LOFIOBJ, 8, 8, {10*8, 8*8, 8, 8}, 3, false, true}},
    {REDBOLT,{LOFIOBJ, 8, 8, {10*8, 8*9, 8, 8}, 3,false, true}},
    {GREENBOLT,{LOFIOBJ, 8, 8, {10*8, 8*10, 8, 8}, 3,false, true}},
    {BLUEBOLT,{LOFIOBJ, 8, 8, {10*8, 8*11, 8, 8}, 3,false, true}},
    {PURPLEBOLT,{LOFIOBJ, 8, 8, {10*8, 8*12, 8, 8}, 3,false, true}},
    {GREENARROW,{LOFIPROJS, 8, 8, {8, 8*7, 8, 8}, 3,false, true}},
    {REDARROW,{LOFIPROJS, 8, 8, {8*2, 8*7, 8, 8}, 3,false, true}},
    {BLUEARROW,{LOFIPROJS, 8, 8, {8*3, 8*7, 8, 8}, 3,false, true}},
    {GOLDARROW,{LOFIPROJS, 8, 8, {8*4, 8*7, 8, 8}, 3,false, true}},
    {SKELETON0,{CHAR8X8RHERO1, 8,8, {0,8*1,8,8}, 4, false, false}},
    {SKELETON1,{CHAR8X8RHERO1, 8,8, {0,8*2,8,8}, 4, false, false}},
    {SKELETON2,{CHAR8X8RHERO1, 8,8, {0,8*3,8,8}, 4, false, false}},
    {SKELETON3,{CHAR8X8RHERO1, 8,8, {0,8*4,8,8}, 4, false, false}},
    {SKELETON4,{CHAR8X8RHERO1, 8,8, {0,8*5,8,8}, 4, false, false}},
    {BLACKSLIME,{LOFICHAR, 8,8, {0,8*11,8,8}, 4, false, false}},
    {BROWNSLIME,{LOFICHAR, 8,8, {8*1,8*11,8,8}, 4, false, false}},
    {GREENSLIME,{LOFICHAR, 8,8, {8*2,8*11,8,8}, 4, false, false}},
    {BOBBY, {LOFICHAR, 7,7, {8*6,8*24+1,7,7}, 4, false, false}},
    {REDKNIGHT0, {LOFICHAR,8,8,{8*9,8*0,8,8},4,false,false}},
    {REDCIRCLEMAGIC,{LOFIOBJ,8,8,{8*1,8*9,8,8},3,false,false}},
    {SHATTERSBOMB, {LOFICHAR,8,8,{8*0,8*28,8,8},4,false,false}},
    {BROWNLOOTBAG, {LOFIOBJ4,8,8,{8*0,8*9,8,8},4,false,false}},
    {PURPLELOOTBAG, {LOFIOBJ4,8,8,{8*1,8*9,8,8},4,false,false}},
    {CYANLOOTBAG, {LOFIOBJ4,8,8,{8*2,8*9,8,8},4,false,false}},
    {BLUELOOTBAG, {LOFIOBJ4,8,8,{8*3,8*9,8,8},4,false,false}},
    {WHITELOOTBAG, {LOFIOBJ4,8,8,{8*4,8*9,8,8},4,false,false}},
};

std::unordered_map<classes, spritedata> classToSpriteData = {
    {WARRIOR, {PLAYERS,8,8,{8*0,WARRIOR*24,8,8},4,false,false}},
    {ARCHER, {PLAYERS,8,8,{8*0,ARCHER*24,8,8},4,false,false}},
    {PRIEST, {PLAYERS,8,8,{8*0,PRIEST*24,8,8},4,false,false}}
};

std::unordered_map<sprites, boxColliders> spriteToBC = {
    {SKELETON0, STANDARD},
    {SKELETON1, STANDARD},
    {SKELETON2, STANDARD},
    {SKELETON3, STANDARD},
    {SKELETON4, STANDARD},
    {REDKNIGHT0, STANDARD},
    {REDCIRCLEMAGIC, CIRCLEMAGIC},
    {SHATTERSBOMB, SHATTERSBOMBBOX}
};

// can be used for ChaseAIComponent and AnimatedChaseAIComponent (unless in the future their fields vary)
// and the neutrals, why not 
struct aiChaseData {
    unsigned short detectRange; // distance in pixels where monster can detect player (ex: path to player)
    unsigned short engageRange; // distance where monster will further engage if possible (ex: shoot)
    unsigned short maxDistance; // closest distance in pixels monster wants to get to player (so monster doens't stand on top of player, or if so they do)

};

std::unordered_map<sprites, aiChaseData> spritesToaiChaseData{
    // not great, but 0 indicates unused variable or one that is derived later from somewhere else
    {SKELETON0, {400, 0, 100}},
    {SKELETON1, {400, 0, 100}},
    {SKELETON2, {400, 0, 100}},
    {SKELETON3, {0, 0, 0}},
    {SKELETON4, {400, 0, 100}},
    {REDKNIGHT0, {150, 0, 50}}
};

struct trapaidata {
    unsigned short engageRange;
    unsigned char iginitionFrame; 
};

std::unordered_map<sprites, trapaidata> spritetotrap = {
    {SHATTERSBOMB, {200, 7}}
};

struct animationData{
    int numFrames;
    double frameSpeedRate;
    int xmin;

};

std::unordered_map<sprites, animationData> spriteToAnimationData = {
    {SKELETON0, {3,0,0}},
    {SKELETON1, {3,0,0}},
    {SKELETON2, {3,0,0}},
    {SKELETON3, {3,0,0}},
    {SKELETON4, {3,0,0}},
    {SHATTERSBOMB, {1,4,0}} // old duration was 3500 ms for the bomb
};
