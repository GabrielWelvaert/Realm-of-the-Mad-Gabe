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
    ADMINCROWN,
    CHICKENTOME,
    SNAKESKINARMOR,
    TWILIGHTGEMSTONE,
    CHICKENSWORD,
    ARCTOME,
    ARCROBE,
    ARCWAND,
    T0STAFF,
    T1STAFF,
    T2STAFF,
    T3STAFF,
    T4STAFF,
    T5STAFF,
    T6STAFF,
    T7STAFF,
    T8STAFF,
    T9STAFF,
    T10STAFF,
    T11STAFF,
    T12STAFF,
    T13STAFF,
    T14STAFF,
    T0DAGGER,
    T1DAGGER,
    T2DAGGER,
    T3DAGGER,
    T4DAGGER,
    T5DAGGER,
    T6DAGGER,
    T7DAGGER,
    T8DAGGER,
    T9DAGGER,
    T10DAGGER,
    T11DAGGER,
    T12DAGGER,
    T13DAGGER,
    T14DAGGER,
    T0SHIELD,
    T1SHIELD,
    T2SHIELD,
    T3SHIELD,
    T4SHIELD,
    T5SHIELD,
    T6SHIELD,
    T7SHIELD,
    T8SHIELD,
    T0SPELL,
    T1SPELL,
    T2SPELL,
    T3SPELL,
    T4SPELL,
    T5SPELL,
    T6SPELL,
    T7SPELL,
    T8SPELL,
    T0CLOAK,
    T1CLOAK,
    T2CLOAK,
    T3CLOAK,
    T4CLOAK,
    T5CLOAK,
    T6CLOAK,
    T7CLOAK,
    T8CLOAK,
    MITHRILSWORD,
    DOOMQUIVER,
    RETRIBUTIONCLOAK,
    CURLYWHIRLYSPELL,
    /*all entries MUST be made exactly above this line- do not add to front*/
    TOTAL_NUMBER_OF_ITEMS
};

enum cardinalDirection{
    N,S,W,E
};

enum enemyCategory{
    ASC, // animated shoot chase 
    AS, // animated shoot
    SC, // shoot chase 
    T, // trap
    CHICKENBOSSAI,
    ARCMAGEBOSSAI,
    GORDONBOSSAI,
    CUBEGODAI,
    KEY,
    MINION,
};

enum parabolicCompletion{
    PARABOLIC_BULLET, //entity is a bullet
    PARABOLIC_MEDUSA_AOE_BOMB, // entity will spawn AOE damage at end of lerp
    PARABOLIC_TRAP,
};

// an incredibly messy and semantically ambiguous enum
enum wallTheme{ //wall themes are used as primary keys for areas/dungeons 
    UDL, // ACTUAL DUNGEON: CASTLE
    AOD,
    NEXUS,
    ORYX1,
    GUILDHALL, //aka snake pit
    VAULT,
    CHICKENLAIR, // ACTUAL DUNGEON: CHICKEN LAIR
    CHANGENAME, // DONT SPAWN AREAS WITH THIS PLEASE! ITS JUST FOR CHANGE NAME 
    CHANGECHAR,
    GORDONSLAIRWALLTHEME, // ACTUAL DUNGEON: GORDONS LAIR
    LOCKEDPORTALTHEME,
    GODLANDS,
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
    // MAGIC, // ex: wand
    // MISSILE, // staves 
    // BOLT, // ex: fire sword
    // BLADE, // ex: low tier swords
    // ARROW, // bows
    STANDARD, // 8x8 players
    // BULLETBOX, //? remove?
    // CIRCLEMAGIC,
    SHATTERSBOMBBOX,
    LOOTBAG,
    WIDE,
    BIG,
    STAR,
    PORTALBOX,
    SCALED7,
    SCALED8,
    SCALED4,
};

// like a primary key for an entity, used to access needed member attributes in tables.h for almost all components
enum sprites{
    // REDMAGIC,
    // BLUEMAGIC,
    // GREENMAGIC,
    // PURPLEMAGIC,
    // DEFAULTBLADE, //low tier swords n daggers
    // WHITEBOLT,
    // REDBOLT,
    // GREENBOLT,
    // BLUEBOLT,
    // PURPLEBOLT,
    // GREENARROW,
    // REDARROW,
    // BLUEARROW,
    // GOLDARROW,
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
    // REDCIRCLEMAGIC,
    SHATTERSBOMB,
    BROWNLOOTBAG,
    PURPLELOOTBAG,
    CYANLOOTBAG,
    BLUELOOTBAG,
    WHITELOOTBAG,
    VAULTCHEST,
    TINYWHITECHICKEN,
    TINYREDCHICKEN,
    COCKATRICE,
    WHITECHICKEN,
    ROOSTER,
    BIGROOSTER,
    BIGTURKEY,
    BOSSCHICKEN,
    ROBOTURKEY,
    ORANGETURKEY,
    YELLOWTURKEY,
    CYANTURKEY,
    NONESPRITE,
    ARCMAGE,
    HELLHOUND,
    IMP0,
    IMP1,
    IMP2,
    IMP3,
    WHITEDEMON,
    SKELETON5,
    REDSTAR,
    // BLUESTAR,
    // WHITESTAR,
    // PURPLESTAR,
    // GREENSTAR,
    // YELLOWSTAR,
    MOUSE0,
    BAT0,
    GORDON,
    // REDFIREBALL,
    // ORYXTEAR,
    SHEEP,
    // ORYXARROW,
    GIGASHEEP,
    SPRITEGOD,
    MEDUSA,
    DJINN,
    ENTGOD,
    BEHOLDER,
    FLYINGBRAIN,
    SLIMEGOD,
    GHOSTGOD,
    GODSPAWNER,
    CUBEGOD,
    SKULLSHRINE,
    MYSTERIOUSCRYSTAL,
    CYANCUBE,
    ORANGECUBE,
    YELLOWCUBE,
    SPRITEMINION
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
    MNOVA,
    VOIDHIT
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
    // ABYSSBRUTE,
    // ABYSSDEMON,
    // ARCHDEMON,
    // BAT,
    // BEHOLDER,
    // BLOB,
    // CAVEPIRATE,
    // CHICKEN,
    // CUBE,
    // CYCLOPS,
    // DARKELF,
    // DEFAULT,
    // DEMON,
    // DJINN,
    // DRAGON,
    // DWARFGOD,
    // DWARF,
    // EGG,
    // ELF,
    // ENT,
    // FLAMINGSKULL,
    // FLAYER,
    // FLYINGBRAIN,
    // GHOSTGOD,
    // GHOST,
    // GOBLIN,
    // GOLEM,
    // GREATERDEMON,
    // GREATERPITSNAKE,
    // HOBBIT,
    // LIZARDGOD,
    // MEDUSA,
    // MINIONOFORYX,
    // MUMMY,
    // NIGHTELF,
    // OGRE,
    // ORC,
    // ORYX,
    // PIRATEKING,
    // PIRATE,
    // PITSNAKE,
    // PYTHON,
    // ROCK,
    // SCORPION,
    // SKELETON,
    // SKULLSHRINE,
    // SLIME,
    // SNAKEQUEEN,
    // SNAKE,
    // SPIDERQUEEN,
    // SPIDER,
    // SPRITEGOD,
    // SPRITE,
    // STONEWALL,
    // TRAP,
    // TREE,
    // UNDEADDWARF,
    // UNDEADHOBBIT,
    // WOODWALL,
    // TESTDUMMY,
    // REDKNIGHT
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

enum monsterSubGroups{
    CUBEGODMINION,
    GODLANDSGOD,
    SKULLSHRINEMINION,
    MYSTERIOUSCRYSTALMINION,
};

enum minionAItypes{
    ORBIT,
    ORBIT_SHOOT,
};

enum groups{
    MONSTER,
    PROJECTILE,
    WALLBOX,
    GUI,
    PLAYER,
    LOOTBAGGROUP,
    DAGGER,
    STAFF,
    SHIELD,
    SPELL,
    CLOAK,
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
    PORTAL,
    /*all consumable group types must be entered below this line*/ 
    HPPOTGROUP, 
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
    BERSERK,
    CONFUSED,
    BLEEDING,
    INVULNERABLE,
    STUNNED,
    INVISIBLE,
    BLIND,
    /* entries must be made above*/
    TOTAL_NUMBER_OF_STATUS_EFFECTS,
    NULL_STATUS_EFFECT
};

enum textureEnums{
    LOFICHAR,
    PLAYERS,
    INVISIBLEPLAYERS,
    LOFIOBJ,
    LOFIPROJS,
    GUIBACKGROUND,
    LOFIENVIRONMENT,
    LOFIENVIRONMENT2,
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
    LOFICHAR2,
    T0SWORDICON,
    T1SWORDICON,
    T2SWORDICON,
    T3SWORDICON,
    T4SWORDICON,
    T5SWORDICON,
    T6SWORDICON,
    T7SWORDICON,
    T8SWORDICON,
    T9SWORDICON,
    T10SWORDICON,
    T11SWORDICON,
    T12SWORDICON,
    T13SWORDICON,
    T14SWORDICON,
    IMPBLADEICON,
    T0BOWICON,
    T1BOWICON,
    T2BOWICON,
    T3BOWICON,
    T4BOWICON,
    T5BOWICON,
    T6BOWICON,
    T7BOWICON,
    T8BOWICON,
    T9BOWICON,
    T10BOWICON,
    T11BOWICON,
    T12BOWICON,
    T13BOWICON,
    T14BOWICON,
    T0WANDICON,
    T1WANDICON,
    T2WANDICON,
    T3WANDICON,
    T4WANDICON,
    T5WANDICON,
    T6WANDICON,
    T7WANDICON,
    T8WANDICON,
    T9WANDICON,
    T10WANDICON,
    T11WANDICON,
    T12WANDICON,
    T13WANDICON,
    T14WANDICON,
    T0ATTRINGICON,
    T1ATTRINGICON,
    T2ATTRINGICON,
    T3ATTRINGICON,
    T4ATTRINGICON,
    T0DEFRINGICON,
    T1DEFRINGICON,
    T2DEFRINGICON,
    T3DEFRINGICON,
    T4DEFRINGICON,
    T0SPDRINGICON,
    T1SPDRINGICON,
    T2SPDRINGICON,
    T3SPDRINGICON,
    T4SPDRINGICON,
    T0VITRINGICON,
    T1VITRINGICON,
    T2VITRINGICON,
    T3VITRINGICON,
    T4VITRINGICON,
    T0WISRINGICON,
    T1WISRINGICON,
    T2WISRINGICON,
    T3WISRINGICON,
    T4WISRINGICON,
    T0DEXRINGICON,
    T1DEXRINGICON,
    T2DEXRINGICON,
    T3DEXRINGICON,
    T4DEXRINGICON,
    T0HPRINGICON,
    T1HPRINGICON,
    T2HPRINGICON,
    T3HPRINGICON,
    T4HPRINGICON,
    T0MPRINGICON,
    T1MPRINGICON,
    T2MPRINGICON,
    T3MPRINGICON,
    T4MPRINGICON,
    T0LIGHTARMORICON,
    T1LIGHTARMORICON,
    T2LIGHTARMORICON,
    T3LIGHTARMORICON,
    T4LIGHTARMORICON,
    T5LIGHTARMORICON,
    T6LIGHTARMORICON,
    T7LIGHTARMORICON,
    T8LIGHTARMORICON,
    T9LIGHTARMORICON,
    T10LIGHTARMORICON,
    T11LIGHTARMORICON,
    T12LIGHTARMORICON,
    T13LIGHTARMORICON,
    T14LIGHTARMORICON,
    T0HEAVYARMORICON,
    T1HEAVYARMORICON,
    T2HEAVYARMORICON,
    T3HEAVYARMORICON,
    T4HEAVYARMORICON,
    T5HEAVYARMORICON,
    T6HEAVYARMORICON,
    T7HEAVYARMORICON,
    T8HEAVYARMORICON,
    T9HEAVYARMORICON,
    T10HEAVYARMORICON,
    T11HEAVYARMORICON,
    T12HEAVYARMORICON,
    T13HEAVYARMORICON,
    T14HEAVYARMORICON,
    T0ROBEICON,
    T1ROBEICON,
    T2ROBEICON,
    T3ROBEICON,
    T4ROBEICON,
    T5ROBEICON,
    T6ROBEICON,
    T7ROBEICON,
    T8ROBEICON,
    T9ROBEICON,
    T10ROBEICON,
    T11ROBEICON,
    T12ROBEICON,
    T13ROBEICON,
    T14ROBEICON,
    T0QUIVERICON,
    T1QUIVERICON,
    T2QUIVERICON,
    T3QUIVERICON,
    T4QUIVERICON,
    T5QUIVERICON,
    T6QUIVERICON,
    T7QUIVERICON,
    T8QUIVERICON,
    T0TOMEICON,
    T1TOMEICON,
    T2TOMEICON,
    T3TOMEICON,
    T4TOMEICON,
    T5TOMEICON,
    T6TOMEICON,
    T7TOMEICON,
    T8TOMEICON,
    T0HELMICON,
    T1HELMICON,
    T2HELMICON,
    T3HELMICON,
    T4HELMICON,
    T5HELMICON,
    T6HELMICON,
    T7HELMICON,
    T8HELMICON,
    ATTACKPENDANTICON,
    HPPOTICON,
    MPPOTICON,
    ATTPOTICON,
    DEFPOTICON,
    DEXPOTICON,
    SPDPOTICON,
    WISPOTICON,
    VITPOTICON,
    LIFEPOTICON,
    MANAPOTICON,
    CABERNETICON,
    FIREWATERICON,
    CHICKENLAIRPORTAL,
    VAULTPORTAL,
    NEXUSPORTAL,
    CHANGENAMEPORTAL,
    CHANGECHARPORTAL,
    PORTALBUTTONBACKGROUND,
    CHARS8X8BEACH,
    CHARS8X8ENCOUNTERS,
    CHARS16X16ENCOUNTERS,
    ADMINCROWNICON,
    LOFIOBJ3,
    CHARS16X16MOUNTAINS1,
    CASTLEPORTAL,
    GORDONSCHAMBERPORTAL,
    HORIZONTALROOMBLOCKWALLS,
    HORIZONTALROOMBLOCKCEILINGS,
    VERTICALROOMBLOCKCEILINGS,
    CHICKENTOMEICON,
    LOFIINTERFACE2,
    STATICHUDARCHER,
    STATICHUDPRIEST,
    STATICHUDWARRIOR,
    MINIMAPFLOOR,
    MINIMAPVEIL,
    SNAKESKINICON,
    LOCKEDPORTAL,
    TWILIGHTGEMSTONEICON,
    CHICKENSWORDICON,
    ARCWANDICON,
    ARCTOMEICON,
    ARCROBEICON,
    T0SHIELDICON,
    T1SHIELDICON,
    T2SHIELDICON,
    T3SHIELDICON,
    T4SHIELDICON,
    T5SHIELDICON,
    T6SHIELDICON,
    T7SHIELDICON,
    T8SHIELDICON,
    T0CLOAKICON,
    T1CLOAKICON,
    T2CLOAKICON,
    T3CLOAKICON,
    T4CLOAKICON,
    T5CLOAKICON,
    T6CLOAKICON,
    T7CLOAKICON,
    T8CLOAKICON,
    T0SPELLICON,
    T1SPELLICON,
    T2SPELLICON,
    T3SPELLICON,
    T4SPELLICON,
    T5SPELLICON,
    T6SPELLICON,
    T7SPELLICON,
    T8SPELLICON,
    T0STAFFICON,
    T1STAFFICON,
    T2STAFFICON,
    T3STAFFICON,
    T4STAFFICON,
    T5STAFFICON,
    T6STAFFICON,
    T7STAFFICON,
    T8STAFFICON,
    T9STAFFICON,
    T10STAFFICON,
    T11STAFFICON,
    T12STAFFICON,
    T13STAFFICON,
    T14STAFFICON,
    T0DAGGERICON,
    T1DAGGERICON,
    T2DAGGERICON,
    T3DAGGERICON,
    T4DAGGERICON,
    T5DAGGERICON,
    T6DAGGERICON,
    T7DAGGERICON,
    T8DAGGERICON,
    T9DAGGERICON,
    T10DAGGERICON,
    T11DAGGERICON,
    T12DAGGERICON,
    T13DAGGERICON,
    T14DAGGERICON,
    STATICHUDWIZARD,
    STATICHUDKNIGHT,
    STATICHUDROGUE,
    MITHRILSWORDICON,
    CURLYWHIRLYSPELLICON,
    RETRIBUTIONCLOAKICON,
    DOOMQUIVERICON,
    GODLANDSPORTAL, 
};

#endif