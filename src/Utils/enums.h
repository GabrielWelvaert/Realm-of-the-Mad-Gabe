#ifndef ENUMS_H
#define ENUMS_H

enum items{
    T0SWORD,
    T1SWORD,
    T2SWORD,
    T3SWORD,
    T4SWORD,
    T5SWORD,
    T6SWORD,
    T7SWORD,
    T8SWORD,
    T9SWORD,
    T10SWORD,
    T11SWORD,
    T12SWORD,
    T13SWORD,
    T14SWORD,
    IMPBLADE,
    T0BOW,
    T1BOW,
    T2BOW,
    T3BOW,
    T4BOW,
    T5BOW,
    T6BOW,
    T7BOW,
    T8BOW,
    T9BOW,
    T10BOW,
    T11BOW,
    T12BOW,
    T13BOW,
    T14BOW,
    T0WAND,
    T1WAND,
    T2WAND,
    T3WAND,
    T4WAND,
    T5WAND,
    T6WAND,
    T7WAND,
    T8WAND,
    T9WAND,
    T10WAND,
    T11WAND,
    T12WAND,
    T13WAND,
    T14WAND,
    T0ATTRING,
    T1ATTRING,
    T2ATTRING,
    T3ATTRING,
    T4ATTRING,
    T0DEFRING,
    T1DEFRING,
    T2DEFRING,
    T3DEFRING,
    T4DEFRING,
    T0SPDRING,
    T1SPDRING,
    T2SPDRING,
    T3SPDRING,
    T4SPDRING,
    T0VITRING,
    T1VITRING,
    T2VITRING,
    T3VITRING,
    T4VITRING,
    T0WISRING,
    T1WISRING,
    T2WISRING,
    T3WISRING,
    T4WISRING,
    T0DEXRING,
    T1DEXRING,
    T2DEXRING,
    T3DEXRING,
    T4DEXRING,
    T0HPRING,
    T1HPRING,
    T2HPRING,
    T3HPRING,
    T4HPRING,
    T0MPRING,
    T1MPRING,
    T2MPRING,
    T3MPRING,
    T4MPRING,
    T0LIGHTARMOR,
    T1LIGHTARMOR,
    T2LIGHTARMOR,
    T3LIGHTARMOR,
    T4LIGHTARMOR,
    T5LIGHTARMOR,
    T6LIGHTARMOR,
    T7LIGHTARMOR,
    T8LIGHTARMOR,
    T9LIGHTARMOR,
    T10LIGHTARMOR,
    T11LIGHTARMOR,
    T12LIGHTARMOR,
    T13LIGHTARMOR,
    T14LIGHTARMOR,
    T0HEAVYARMOR,
    T1HEAVYARMOR,
    T2HEAVYARMOR,
    T3HEAVYARMOR,
    T4HEAVYARMOR,
    T5HEAVYARMOR,
    T6HEAVYARMOR,
    T7HEAVYARMOR,
    T8HEAVYARMOR,
    T9HEAVYARMOR,
    T10HEAVYARMOR,
    T11HEAVYARMOR,
    T12HEAVYARMOR,
    T13HEAVYARMOR,
    T14HEAVYARMOR,
    T0ROBE,
    T1ROBE,
    T2ROBE,
    T3ROBE,
    T4ROBE,
    T5ROBE,
    T6ROBE,
    T7ROBE,
    T8ROBE,
    T9ROBE,
    T10ROBE,
    T11ROBE,
    T12ROBE,
    T13ROBE,
    T14ROBE,
    T0QUIVER,
    T1QUIVER,
    T2QUIVER,
    T3QUIVER,
    T4QUIVER,
    T5QUIVER,
    T6QUIVER,
    T7QUIVER,
    T8QUIVER,
    T0TOME,
    T1TOME,
    T2TOME,
    T3TOME,
    T4TOME,
    T5TOME,
    T6TOME,
    T7TOME,
    T8TOME,
    T0HELM,
    T1HELM,
    T2HELM,
    T3HELM,
    T4HELM,
    T5HELM,
    T6HELM,
    T7HELM,
    T8HELM,
    ATTACKPENDANT,
    HPPOT,
    MPPOT,
    ATTPOT,
    DEFPOT,
    DEXPOT,
    SPDPOT,
    WISPOT,
    VITPOT,
    LIFEPOT,
    MANAPOT,
    CABERNET,
    FIREWATER,
};


enum enemyCategory{
    ASC, // animated shoot chase 
    AS, // animated shoot
    SC, // shoot chase 
    T, // trap
};

enum wallTheme{
    UDL,
    AOD,
    NEXUS,
    ORYX1,
    GUILDHALL
};

enum itemPositions{
    NOPOSITION,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT
};

enum stats{ // used to get values via indexing in statSys lvlup
    HP,
    MP,
    ATTACK,
    DEFENSE,
    SPEED,
    DEXTERITY,
    VITALITY,
    WISDOM,
    XP, // used in UpdateDisplayStatTextSystem only
    LVL // used in UpdateDisplayStatTextSystem only
};

enum boxColliders{ //ALSO USED FOR MAKING SPRITECOMPONENTS
    MAGIC, // ex: wand
    MISSILE, // staves 
    BOLT, // ex: fire sword
    BLADE, // ex: low tier swords
    ARROW, // bows
    STANDARD, // 8x8 players
    LARGE, // TODO: 32x32 enemies
    BULLETBOX, //? remove?
    CIRCLEMAGIC,
    SHATTERSBOMBBOX,
    LOOTBAG
};

// like a primary key for an entity, used to access needed member attributes in tables.h for almost all components
enum sprites{
    REDMAGIC,
    BLUEMAGIC,
    GREENMAGIC,
    PURPLEMAGIC,
    DEFAULTBLADE, //low tier swords n daggers
    WHITEBOLT,
    REDBOLT,
    GREENBOLT,
    BLUEBOLT,
    PURPLEBOLT,
    GREENARROW,
    REDARROW,
    BLUEARROW,
    GOLDARROW,
    SKELETON0,
    SKELETON1,
    SKELETON2,
    SKELETON3,
    SKELETON4,
    BLACKSLIME,
    GREENSLIME,
    BROWNSLIME,
    BOBBY,
    REDKNIGHT0,
    REDCIRCLEMAGIC,
    SHATTERSBOMB,
    BROWNLOOTBAG,
    PURPLELOOTBAG,
    CYANLOOTBAG,
    BLUELOOTBAG,
    WHITELOOTBAG,
};

enum soundEnums{
    MAGICSHOOT,
    ARROWSHOOT,
    DAGGERSWING,
    BLADESWING,
    ARCHERHIT,
    KNIGHTHIT,
    PALADINHIT,
    PRIESTHIT,
    ROGUEHIT,
    WARRIORHIT,
    WIZARDHIT,
    ARCHERDEATH,
    KNIGHTDEATH,
    PALADINDEATH,
    PRIESTDEATH,
    ROGUEDEATH,
    WARRIORDEATH,
    WIZARDDEATH,
    BUTTON,
    DEATH,
    KILLVOID,
    ERROR,
    INVENTORY,
    LEVELUP,
    LOOT,
    NOMANA,
    UNLOCKVOID,
    POTION,
    ABYSSBRUTESDEATH,
    ABYSSBRUTESHIT,
    ABYSSDEMONSDEATH,
    ABYSSDEMONSHIT,
    ARCHDEMONDEATH,
    ARCHDEMONHIT,
    BATSDEATH,
    BATSHIT,
    BEHOLDERDEATH,
    BEHOLDERHIT,
    BLOBSDEATH,
    BLOBSHIT,
    CAVEPIRATESDEATH,
    CAVEPIRATESHIT,
    CHICKENDEATH,
    CHICKENHIT,
    CUBESDEATH,
    CUBESHIT,
    CYCLOPSDEATH,
    CYCLOPSHIT,
    DARKELVESDEATH,
    DARKELVESHIT,
    DEFAULTDEATH,
    DEFAULTHIT,
    DEMONSDEATH,
    DEMONSHIT,
    DJINNDEATH,
    DJINNHIT,
    DRAGONSDEATH,
    DRAGONSHIT,
    DWARFGODDEATH,
    DWARFGODHIT,
    DWARVESDEATH,
    DWARVESHIT,
    EGGSDEATH,
    EGGSHIT,
    ELVESDEATH,
    ELVESHIT,
    ENTSDEATH,
    ENTSHIT,
    FLAMINGSKULLSDEATH,
    FLAMINGSKULLSHIT,
    FLAYERSDEATH,
    FLAYERSHIT,
    FLYINGBRAINDEATH,
    FLYINGBRAINHIT,
    GHOSTSDEATH,
    GHOSTSHIT,
    GHOSTGODDEATH,
    GHOSTGODHIT,
    GOBLINSDEATH,
    GOBLINSHIT,
    GOLEMSDEATH,
    GOLEMSHIT,
    GREATERDEMONSDEATH,
    GREATERDEMONSHIT,
    GREATERPITSNAKESDEATH,
    GREATERPITSNAKESHIT,
    HOBBITSDEATH,
    HOBBITSHIT,
    LIZARDGODDEATH,
    LIZARDGODHIT,
    MEDUSADEATH,
    MEDUSAHIT,
    MINIONOFORYXDEATH,
    MINIONOFORYXHIT,
    MUMMIESDEATH,
    MUMMIESHIT,
    NIGHTELVESDEATH,
    NIGHTELVESHIT,
    OGRESDEATH,
    OGRESHIT,
    ORCSDEATH,
    ORCSHIT,
    ORYXDEATH,
    ORYXHIT,
    PIRATESDEATH,
    PIRATESHIT,
    PIRATEKINGDEATH,
    PIRATEKINGHIT,
    PITSNAKESDEATH,
    PITSNAKESHIT,
    PYTHONSDEATH,
    PYTHONSHIT,
    REDKNIGHTDEATH,
    REDKNIGHTHIT,
    ROCKSDEATH,
    ROCKSHIT,
    SCORPIONSDEATH,
    SCORPIONSHIT,
    SKELETONSDEATH,
    SKELETONSHIT,
    SKULLSHRINEDEATH,
    SKULLSHRINEHIT,
    SLIMESDEATH,
    SLIMESHIT,
    SNAKESDEATH,
    SNAKESHIT,
    SNAKEQUEENDEATH,
    SNAKEQUEENHIT,
    SPIDERSDEATH,
    SPIDERSHIT,
    SPIDERQUEENDEATH,
    SPIDERQUEENHIT,
    SPRITESDEATH,
    SPRITESHIT,
    SPRITEGODDEATH,
    SPRITEGODHIT,
    STONEWALLSDEATH,
    STONEWALLSHIT,
    TRAPSDEATH,
    TRAPSHIT,
    TREESDEATH,
    TREESHIT,
    UNDEADDWARVESDEATH,
    UNDEADDWARVESHIT,
    UNDEADHOBBITSDEATH,
    UNDEADHOBBITSHIT,
    WOODENWALLSDEATH,
    WOODENWALLSHIT,
};

enum textureEnums{
    LOFICHAR,
    PLAYERS,
    LOFIOBJ,
    LOFIPROJS,
    GUIBACKGROUND,
    LOFIENVIRONMENT,
    LOFIOBJ4,
    TEMPMINIMAP,
    LOFIINTERFACEBIG,
    CHAR8X8RHERO1,
    INVENTORYICONS,
    LOFIOBJ5,
    LOFIOBJ2,
    BIGFLOOR,
    BIGWALL,
    BIGCEILING,
    LOFIOBJ5B,
    LOFIOBJ6,
    STATICHUD,
    STATICHUD2,
    BAGSLOTS,
    MAINMENUBG,
    PLAYBAR,
    LOFICHAR2

};

// used to calculate spriteatlasoffset (if applicable) 
// and to construct stats, sprite
enum classes{
    ROGUE, 
    ARCHER,
    WIZARD,
    PRIEST,
    WARRIOR,
    KNIGHT,
    PALADIN,
    ASSASSIN,
    NECROMANCER,
    HUNTRESS,
    MYSTIC,
    TRICKSTER,
    SORCERER,
    ABYSSBRUTE,
    ABYSSDEMON,
    ARCHDEMON,
    BAT,
    BEHOLDER,
    BLOB,
    CAVEPIRATE,
    CHICKEN,
    CUBE,
    CYCLOPS,
    DARKELF,
    DEFAULT,
    DEMON,
    DJINN,
    DRAGON,
    DWARFGOD,
    DWARF,
    EGG,
    ELF,
    ENT,
    FLAMINGSKULL,
    FLAYER,
    FLYINGBRAIN,
    GHOSTGOD,
    GHOST,
    GOBLIN,
    GOLEM,
    GREATERDEMON,
    GREATERPITSNAKE,
    HOBBIT,
    LIZARDGOD,
    MEDUSA,
    MINIONOFORYX,
    MUMMY,
    NIGHTELF,
    OGRE,
    ORC,
    ORYX,
    PIRATEKING,
    PIRATE,
    PITSNAKE,
    PYTHON,
    ROCK,
    SCORPION,
    SKELETON,
    SKULLSHRINE,
    SLIME,
    SNAKEQUEEN,
    SNAKE,
    SPIDERQUEEN,
    SPIDER,
    SPRITEGOD,
    SPRITE,
    STONEWALL,
    TRAP,
    TREE,
    UNDEADDWARF,
    UNDEADHOBBIT,
    WOODWALL,
    TESTDUMMY,
    REDKNIGHT
};

enum movements{
    NONE,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    UPLEFT,
    UPRIGHT,
    DOWNLEFT,
    DOWNRIGHT,
};

enum sidehit{
    TOPSIDE,
    BOTTOMSIDE,
    LEFTSIDE,
    RIGHTSIDE,
    NONESIDE
};

enum tags{

};

enum groups{
    MONSTER,
    PROJECTILE,
    WALLBOX,
    GUI,
    PLAYER,
    LOOTBAGGROUP,
    SWORD,
    WAND,
    BOW,
    QUIVER,
    HELM,
    TOME,
    ROBE,
    LIGHTARMOR,
    HEAVYARMOR,
    RING,
    HPPOTGROUP, // keep the groups at the end for logic in processInput
    MPPOTGROUP,
    ATTPOTGROUP,
    DEFPOTGROUP,
    DEXPOTGROUP,
    SPDPOTGROUP,
    WISPOTGROUP,
    VITPOTGROUP,
    LIFEPOTGROUP,
    MANAPOTGROUP,
    CABERNETGROUP,
    FIREWATERGROUP,

};

enum statuses{
    QUIET,
    SLOWED,
    PARALYZE,
    SPEEDY,
    BERSERK
};


#endif