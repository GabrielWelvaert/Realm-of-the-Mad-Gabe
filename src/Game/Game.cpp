#include "Game.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_mixer.h"
#include <iostream>
#include "../../libs/glm/glm.hpp"
#include <random>
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardMovementSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include <fstream>
#include <sstream>
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifeCycleSystem.h"
#include "../Systems/ArtificialIntelligenceSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/StatSystem.h"
#include <filesystem>
#include "../Systems/DynamicUIRenderSystem.h"
#include "../Systems/UpdateDisplayStatTextSystem.h"
#include "../Utils/colors.h"
#include "../Systems/ProjectileMovementSystem.h"
#include "../Systems/InteractUISystem.h"
#include "../Systems/LootBagSystem.h"
#include "../Systems/RenderMouseBoxSystem.h"
#include "../Systems/ItemMovementSystem.h"
#include "../Utils/factory.h"
#include "../Systems/AbilitySystem.h"
#include "../Events/StatusEffectEvent.h"
#include "../Systems/StatusEffectSystem.h"

int Game::windowWidth = 1000;
int Game::windowHeight = 750;
int Game::mapWidth; //in pixels, initialized when loading level
int Game::mapheight; //in pixels, initialized when loading level
int Game::mouseX;
int Game::mouseY;

Game::Game(){
    isRunning = false;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    eventBus = std::make_unique<EventBus>();
    factory = std::make_unique<Factory>();
}

Game::~Game(){
    // std::cout << "game destructor called" << std::endl;
}

//initialize SDL stuff 
void Game::Initialize(){
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); // shoutout logan h. for finding SDL_VIDEODRIVER 
    Mix_AllocateChannels(32);
    Mix_Volume(-1, 64);
    Mix_VolumeMusic(64);

    window = SDL_CreateWindow(
        "Realm of the Mad Gabe", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    SDL_SetWindowResizable(window, SDL_FALSE);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //camera stuff
    camera.x = 0;
    camera.y = 0;
    camera.w = windowHeight; 
    camera.h = windowHeight;

    // algorithm for window icon 
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister pseudo-random number generator
    SDL_Surface* atlas = IMG_Load("assets/images/lofi_portrait.png");
    std::uniform_int_distribution<> disx(0, 15); 
    std::uniform_int_distribution<> disy(0, 27); 
    SDL_Rect rectempty1 = {8*15,8*15,8,8}; //x,y,w,h
    SDL_Surface* empty1 = SDL_CreateRGBSurface(0, rectempty1.w, rectempty1.h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(atlas, &rectempty1, empty1, NULL);
    SDL_Rect portraitRect = {8*disx(gen),8*disy(gen),8,8}; //x,y,w,h
    SDL_Surface* iconsmall = SDL_CreateRGBSurface(0, portraitRect.w, portraitRect.h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(atlas, &portraitRect, iconsmall, NULL);
    // while iconsmall is identical to an empty portrait, re-roll a tile from portrait atlas
    while (memcmp(empty1->pixels, iconsmall->pixels, empty1->w * empty1->h * empty1->format->BytesPerPixel) == 0){
        portraitRect = {8*disx(gen),8*disy(gen),8,8}; //x,y,w,h
        iconsmall = SDL_CreateRGBSurface(0, portraitRect.w, portraitRect.h, 32, 0, 0, 0, 0);
        SDL_BlitSurface(atlas, &portraitRect, iconsmall, NULL); 
    }
    SDL_Surface* iconlarge = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            int smallX = x / 4;  
            int smallY = y / 4;  
            Uint32 pixel = ((Uint32*)iconsmall->pixels)[smallY * 8 + smallX];
            ((Uint32*)iconlarge->pixels)[y * 32 + x] = pixel;
        }
    }
    SDL_SetWindowIcon(window, iconlarge);
    SDL_FreeSurface(iconsmall);
    SDL_FreeSurface(iconlarge);
    SDL_FreeSurface(atlas);

    if(debug){
        SDL_RendererInfo rendererInfo;
        SDL_GetRendererInfo(renderer, &rendererInfo);
        // std::cout << "SDL_GetCurrentVideoDriver() = " << SDL_GetCurrentVideoDriver() << std::endl;
        // std::cout << "SDL_GetRendererInfo() = " << rendererInfo.name << std::endl;
    }

    isRunning = true; 
}

void Game::Run(){
    Setup();
    while (isRunning){
        ProcessInput();
        Update();
        Render();
    }
}

// some things used by ProcessInput:
std::bitset<5> keysPressed; 
std::unordered_map<SDL_Keycode, int> keyindex = {
    {SDLK_w, 0}, 
    {SDLK_a, 1}, 
    {SDLK_s, 2},  
    {SDLK_d, 3},
    //keysPressed[4] is for mouse button (any) but its not a SDL_KeyCode! 
};
std::bitset<8> inventoryUses; // used to enforce player can use a inventory slot ONCE per ProcessInput (as to not clog eventbus)
bool space = false;
bool shift = false;
SDL_Keycode key;
unsigned int startTime;
const unsigned int MSToReadInput = 20;
int invetoryNumber;

void Game::ProcessInput(){
    startTime = SDL_GetTicks();
    inventoryUses.reset(); 
    // DO NOT RESET KEYSPRESSED!! 

    while(SDL_GetTicks() - startTime < MSToReadInput){
        SDL_Event sdlEvent;
        while(SDL_PollEvent(&sdlEvent)) {
            key = sdlEvent.key.keysym.sym;
            switch (sdlEvent.type){
                case SDL_QUIT: 
                    isRunning = false;
                    break;
                case SDL_KEYDOWN:
                    if(key == SDLK_m){
                        assetStore->PlayMusic("ost");
                    }
                    else if(keyindex.find(key) != keyindex.end()){ // WASD PRESSED
                        // set WASD in keysPressed! 
                        keysPressed[keyindex[key]] = true;
                    } else if(key == SDLK_SPACE){ // SPACE PRESSED
                        space = true;
                    } else if(static_cast<int>(key) >= 49 && static_cast<int>(key) <= 56){ // 1-8 PRESSED
                        invetoryNumber = static_cast<int>(key)-49;
                        if(!inventoryUses[invetoryNumber]){
                            inventoryUses[invetoryNumber] = true;  
                            const auto& inventory = player.GetComponent<PlayerItemsComponent>().inventory;
                            if(inventory.find(invetoryNumber+1) != inventory.end()){
                                const auto& itemEnum = inventory.at(invetoryNumber+1).GetComponent<ItemComponent>().itemEnum;
                                if(static_cast<int>(itemToGroup.at(itemEnum)) >= 16){ // magic number, start of end of group enums which contains consumable items
                                    eventBus->EmitEvent<DrinkConsumableEvent>(player, itemEnum, registry, assetStore, eventBus, invetoryNumber+1);    
                                } else {
                                    assetStore->PlaySound(ERROR);
                                }
                            }
                        }
                    } else if(key == SDLK_LSHIFT || key == SDLK_RSHIFT){ // SHIFT PRESSED!
                        shift = true;
                    } else if(key == SDLK_9) {
                        glm::vec2 spawnpoint = {mouseX + camera.x, mouseY + camera.y};
                        Entity lootbag = factory->creatLootBag(registry, spawnpoint, WHITELOOTBAG);
                        factory->createItemInBag(registry, T1QUIVER, lootbag);
                        factory->createItemInBag(registry, T2QUIVER, lootbag);
                        factory->createItemInBag(registry, T3QUIVER, lootbag);
                        factory->createItemInBag(registry, T5QUIVER, lootbag);
                        factory->createItemInBag(registry, T4HELM, lootbag);
                        factory->createItemInBag(registry, T5HELM, lootbag);
                        factory->createItemInBag(registry, T6HELM, lootbag);
                        factory->createItemInBag(registry, T7HELM, lootbag);
                    } else if(key == SDLK_0){
                        eventBus->EmitEvent<StatusEffectEvent>(player, PARALYZE, eventBus, 3000);
                    } else if(key == SDLK_MINUS){
                        glm::vec2 spawnpoint = {mouseX + camera.x, mouseY + camera.y};
                        std::vector<sprites> monsters = {SKELETON0, SKELETON1, SKELETON2, SKELETON3, SKELETON4, REDKNIGHT0, SHATTERSBOMB};
                        int index = RNG.randomFromRange(0, monsters.size()-1);
                        factory->spawnMonster(registry, spawnpoint, REDKNIGHT0);
                        // if(monster.HasComponent<StatusEffectComponent>()){
                        //     eventBus->EmitEvent<StatusEffectEvent>(monster, PARALYZE, eventBus, 30000);    
                        // }
                    }
                    break;
                case SDL_KEYUP:
                    if(keyindex.find(key) != keyindex.end()){
                        keysPressed[keyindex[key]] = false;
                    } else if(key == SDLK_SPACE){
                        space = false;
                    } else if(key == SDLK_LSHIFT || key == SDLK_RSHIFT){
                        shift = false;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // std::cout << mouseX << " " << mouseY << std::endl;
                    keysPressed[4] = true;
                    break;
                case SDL_MOUSEBUTTONUP: //remove this ?
                    keysPressed[4] = false;
                    break;
            }
        }
    }
    //get mouse pos at end of 20ms block so we shoot to most recent position 
    SDL_GetMouseState(&Game::mouseX, &Game::mouseY);
    if(keysPressed[4] && mouseX <= 750){
        // SDL_GetMouseState(&Game::mouseX, &Game::mouseY);
        player.GetComponent<ProjectileEmitterComponent>().isShooting = true;
    } else {
        player.GetComponent<ProjectileEmitterComponent>().isShooting = false;
    }

}
struct Vec2Comparator { // used in LoadTilemap algorithm
    bool operator()(const glm::vec2& a, const glm::vec2& b) const{
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
};

struct Vec2Hash { // used in LoadTileMap algorithm
    std::size_t operator()(const glm::vec2& v) const {
        return std::hash<float>{}(v.x) ^ (std::hash<float>{}(v.y) << 1);
    }
};

/*
LoadTileMap version 2.
Params: 1) wallTheme enum 2) path to .map file
creates three big textures for floor, ceiling, and wall tiles 
also makes the boxCollider entities for the walls (as separate entities!) 
*/
void Game::LoadTileMap(const wallTheme& wallTheme, const std::string& pathToMapFile){
    auto wallData = wallThemeToWallData.at(wallTheme);
    SDL_Texture * spriteAtlasTexture = assetStore->GetTexture(wallData.texture);
    std::vector<glm::ivec2> walls = wallData.walls;
    glm::ivec2 ceiling = walls.back(); // last pair in walls is ceiling 
    const int tileSize = 8;
    const float tileScale = 8.0;
    Entity bigFloorEntity = registry->CreateEntity();  // these big textures will be large sprites with transform.position = origin
    Entity bigWallEntity = registry->CreateEntity();
    Entity bigCeilingEntity = registry->CreateEntity();
    bigFloorEntity.AddComponent<TransformComponent>(glm::vec2(0.0,0.0), glm::vec2(tileScale, tileScale));
    bigWallEntity.AddComponent<TransformComponent>(glm::vec2(0.0,0.0), glm::vec2(tileScale, tileScale));
    bigCeilingEntity.AddComponent<TransformComponent>(glm::vec2(0.0,0.0), glm::vec2(tileScale, tileScale));
    int x = 0; // x value from given position in map
    int y = 0; // y value from given position in map
    std::vector<glm::ivec2> wallCoordinates; // used to create contiguous boxColliders later
    std::unordered_map<glm::ivec2, glm::vec2, Vec2Hash> coordinateToPos; // used to create contiguous boxColliders later
    std::unordered_set<glm::vec2, Vec2Hash> wallCoordinatesHashSet; // used to create contiguous boxColliders later
    std::ifstream file(pathToMapFile); // counting rows and columns in .map file to find map width and height
    char c;
    int rows = 1;
    int columns = 1;
    while(file.get(c)){
        if(c == ',' && rows == 1){
            columns++;
        }
        if(c == '\n'){
            rows++;
        }
    }
    mapWidth = columns * tileSize * tileScale;  
    mapheight = rows * tileSize * tileScale;
    file.close();
    SDL_Texture * bigFloorTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mapWidth, mapheight);
    SDL_Texture * bigWallTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mapWidth, mapheight);
    SDL_Texture * bigCeilingTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mapWidth, mapheight);
    SDL_SetTextureBlendMode(bigFloorTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(bigWallTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(bigCeilingTexture, SDL_BLENDMODE_BLEND);
    SDL_Rect srcRect; 
    SDL_Rect dstRect;
    SDL_Rect srcAlpha = {wallData.alpha.x*tileSize, wallData.alpha.y*tileSize, tileSize, tileSize};
    std::fstream map(pathToMapFile);
    int ypos = -1;
    for (std::string line; std::getline(map, line, '\n');){
        int xpos = -1;
        ypos++;
        std::stringstream stream;
        stream << line;
        for (std::string tileid; std::getline(stream, tileid, ',');){
            xpos++;
            int value = std::stoi(tileid);
            x = value % 10; //extract ones digit as x-place
            y = value / 10; //extract tens digit as y-place
            srcRect = {x*tileSize,y*tileSize,tileSize,tileSize};
            dstRect = {xpos*tileSize, ypos*tileSize, tileSize, tileSize};
            glm::ivec2 currentCoord = {y,x};
            bool isFloor = true;
            for(auto wall: walls){
                if(currentCoord == wall){ // wall or ceiling tile 
                    isFloor = false;
                    if(currentCoord == ceiling){ // ceiling tile
                        SDL_SetRenderTarget(renderer, bigCeilingTexture);
                        SDL_RenderCopy(renderer, spriteAtlasTexture, &srcRect, &dstRect);
                        SDL_SetRenderTarget(renderer, nullptr);
                        SDL_SetRenderTarget(renderer, bigWallTexture);
                        SDL_RenderCopy(renderer, spriteAtlasTexture, &srcAlpha, &dstRect);
                        SDL_SetRenderTarget(renderer, nullptr);
                        SDL_SetRenderTarget(renderer, bigFloorTexture);
                        SDL_RenderCopy(renderer, spriteAtlasTexture, &srcAlpha, &dstRect);
                        SDL_SetRenderTarget(renderer, nullptr);
                    } else { // wall tile
                        wallCoordinates.push_back(glm::ivec2(xpos,ypos)); // storing some wall information so I can make boxColliders there 
                        coordinateToPos.insert({glm::ivec2(xpos,ypos), glm::vec2(xpos*tileSize*tileScale, ypos*tileSize*tileScale)});
                        wallCoordinatesHashSet.insert(glm::ivec2(xpos,ypos));
                        SDL_SetRenderTarget(renderer, bigWallTexture);
                        SDL_RenderCopy(renderer, spriteAtlasTexture, &srcRect, &dstRect);
                        SDL_SetRenderTarget(renderer, nullptr);
                        SDL_SetRenderTarget(renderer, bigCeilingTexture);
                        SDL_RenderCopy(renderer, spriteAtlasTexture, &srcAlpha, &dstRect);
                        SDL_SetRenderTarget(renderer, nullptr);
                        SDL_SetRenderTarget(renderer, bigFloorTexture);
                        SDL_RenderCopy(renderer, spriteAtlasTexture, &srcAlpha, &dstRect);
                        SDL_SetRenderTarget(renderer, nullptr);
                    }
                }
            }
            if(isFloor){ // floor tile
                SDL_SetRenderTarget(renderer, bigFloorTexture);
                SDL_RenderCopy(renderer, spriteAtlasTexture, &srcRect, &dstRect);
                SDL_SetRenderTarget(renderer, nullptr);
                SDL_SetRenderTarget(renderer, bigCeilingTexture);
                SDL_RenderCopy(renderer, spriteAtlasTexture, &srcAlpha, &dstRect);
                SDL_SetRenderTarget(renderer, nullptr);
                SDL_SetRenderTarget(renderer, bigWallTexture);
                SDL_RenderCopy(renderer, spriteAtlasTexture, &srcAlpha, &dstRect);
                SDL_SetRenderTarget(renderer, nullptr);
                
            }
            // todo if(isAlpha)
        }
    }
    assetStore->AddTexture(renderer, BIGWALL, bigWallTexture);
    assetStore->AddTexture(renderer, BIGCEILING, bigCeilingTexture);
    assetStore->AddTexture(renderer, BIGFLOOR, bigFloorTexture);
    bigWallEntity.AddComponent<SpriteComponent>(BIGWALL, mapWidth, mapheight, 3, 0,0,0);
    bigCeilingEntity.AddComponent<SpriteComponent>(BIGCEILING, mapWidth, mapheight, 9, 0,0,0);
    bigFloorEntity.AddComponent<SpriteComponent>(BIGFLOOR, mapWidth, mapheight, 0, 0,0,0);
    SDL_RenderClear(renderer);
    map.close();
    // adding boxComponent entities where walls are: (unoptomized algorithm; not at play-time so doens't matter much)
    std::set<glm::ivec2, Vec2Comparator> Xclusters;
    std::set<glm::ivec2, Vec2Comparator> solos; 
    std::vector<glm::ivec2> group;
    int scale = tileSize*tileScale;
    // x-clusters
    for(int i = 0; i < wallCoordinates.size()-1; i++){
        if(wallCoordinates[i].x == wallCoordinates[i+1].x - 1 && wallCoordinates[i].y == wallCoordinates[i+1].y){
            group.push_back(wallCoordinates[i]);
            Xclusters.insert(wallCoordinates[i]);
            while(wallCoordinates[i].x == wallCoordinates[i+1].x - 1 && wallCoordinates[i].y == wallCoordinates[i+1].y){
                group.push_back(wallCoordinates[i+1]);
                Xclusters.insert(wallCoordinates[i+1]);
                i++;
            }
            const auto& position = coordinateToPos.at(group[0]);
            Entity wallbox = registry->CreateEntity();
            wallbox.AddComponent<TransformComponent>(position);
            wallbox.AddComponent<BoxColliderComponent>(group.size() * scale, scale);
            wallbox.Group(WALLBOX);
            group.clear();
        } else { // is this wall alone? 
            glm::vec2 above = {wallCoordinates[i].x,wallCoordinates[i].y - 1};
            glm::vec2 below = {wallCoordinates[i].x,wallCoordinates[i].y + 1};
            if(wallCoordinatesHashSet.find(above) == wallCoordinatesHashSet.end() && wallCoordinatesHashSet.find(below) == wallCoordinatesHashSet.end()){
                solos.insert(wallCoordinates[i]);  
            }
        }
    }
    std::sort(wallCoordinates.begin(), wallCoordinates.end(), [](const glm::vec2& a, const glm::vec2& b) {
        if (a.x != b.x) {return a.x < b.x;}
        return a.y < b.y; 
    });
    // y-clusters
    for(int i = 0; i < wallCoordinates.size()-1; i++){
        if(wallCoordinates[i].y == wallCoordinates[i+1].y - 1 && wallCoordinates[i].x == wallCoordinates[i+1].x){
            if(Xclusters.find(wallCoordinates[i]) == Xclusters.end()){
                group.push_back(wallCoordinates[i]);
            }
            while(wallCoordinates[i].y == wallCoordinates[i+1].y - 1 && wallCoordinates[i].x == wallCoordinates[i+1].x){
                if(Xclusters.find(wallCoordinates[i+1]) == Xclusters.end()){
                    group.push_back(wallCoordinates[i+1]);
                }
                i++;
            }
            const auto& position = coordinateToPos.at(group[0]);
            Entity wallbox = registry->CreateEntity();
            wallbox.AddComponent<TransformComponent>(position);
            wallbox.AddComponent<BoxColliderComponent>(scale, group.size() * scale);
            wallbox.Group(WALLBOX);
            group.clear();
        } else { // is this wall alone? 
            glm::vec2 left = {wallCoordinates[i].x-1,wallCoordinates[i].y};
            glm::vec2 right = {wallCoordinates[i].x+1,wallCoordinates[i].y};
            if(wallCoordinatesHashSet.find(left) == wallCoordinatesHashSet.end() && wallCoordinatesHashSet.find(right) == wallCoordinatesHashSet.end()){
                solos.insert(wallCoordinates[i]);  
            }
        }
    }
    // solo walls 
    for(const auto& x: solos){
        const auto& position = coordinateToPos.at(x);
        Entity wallbox = registry->CreateEntity();
        wallbox.AddComponent<TransformComponent>(position);
        wallbox.AddComponent<BoxColliderComponent>(scale, scale);
        wallbox.Group(WALLBOX);
    }
}

void Game::PopulateAssetStore(){
    assetStore->AddTexture(renderer, LOFICHAR, "./assets/images/lofi_char.png"); 
    assetStore->AddTexture(renderer, PLAYERS, "./assets/images/players.png");
    assetStore->AddTexture(renderer, LOFIOBJ, "./assets/images/lofiObj.png"); //has projectiles and stuff
    assetStore->AddTexture(renderer, LOFIPROJS, "./assets/images/lofiProjs.png"); //arrows etc
    assetStore->AddTexture(renderer, GUIBACKGROUND, "./assets/images/guibackground.png");
    assetStore->AddTexture(renderer, LOFIOBJ4, "./assets/images/lofiObj4.png");
    assetStore->AddTexture(renderer, TEMPMINIMAP, "./assets/images/todominimap.png");
    assetStore->AddTexture(renderer, LOFIINTERFACEBIG, "./assets/images/lofiInterfaceBig.png");
    assetStore->AddTexture(renderer, CHAR8X8RHERO1, "./assets/images/chars8x8rHero1.png");
    assetStore->AddTexture(renderer, INVENTORYICONS, "./assets/images/inventoryIcons.png");
    assetStore->AddTexture(renderer, LOFIOBJ5, "./assets/images/lofiObj5.png");
    assetStore->AddTexture(renderer, LOFIOBJ2, "./assets/images/lofiObj2.png");
    assetStore->AddTexture(renderer, LOFIENVIRONMENT, "./assets/images/lofiEnvironment.png");
    assetStore->AddTexture(renderer, LOFIOBJ5B, "./assets/images/lofiObj5b.png");
    assetStore->AddTexture(renderer, LOFIOBJ6, "./assets/images/lofiObj6.png");
    
    assetStore->AddSound(MAGICSHOOT, "./assets/sounds/weapon_sounds/magicShoot.wav");
    assetStore->AddSound(ARROWSHOOT, "./assets/sounds/weapon_sounds/arrowShoot.wav");
    assetStore->AddSound(DAGGERSWING, "./assets/sounds/weapon_sounds/daggerSwing.wav");
    assetStore->AddSound(BLADESWING, "./assets/sounds/weapon_sounds/bladeSwing.wav");

    assetStore->AddSound(ARCHERHIT, "./assets/sounds/characters/archer_hit.wav");
    assetStore->AddSound(KNIGHTHIT, "./assets/sounds/characters/knight_hit.wav");
    assetStore->AddSound(PALADINHIT, "./assets/sounds/characters/paladin_hit.wav");
    assetStore->AddSound(PRIESTHIT, "./assets/sounds/characters/priest_hit.wav");
    assetStore->AddSound(ROGUEHIT, "./assets/sounds/characters/rogue_hit.wav");
    assetStore->AddSound(WARRIORHIT, "./assets/sounds/characters/warrior_hit.wav");
    assetStore->AddSound(WIZARDHIT, "./assets/sounds/characters/wizard_hit.wav");
    assetStore->AddSound(ARCHERDEATH, "./assets/sounds/characters/archer_death.wav");
    assetStore->AddSound(KNIGHTDEATH, "./assets/sounds/characters/knight_death.wav");
    assetStore->AddSound(PALADINDEATH, "./assets/sounds/characters/paladin_death.wav");
    assetStore->AddSound(PRIESTDEATH, "./assets/sounds/characters/priest_death.wav");
    assetStore->AddSound(ROGUEDEATH, "./assets/sounds/characters/rogue_death.wav");
    assetStore->AddSound(WARRIORDEATH, "./assets/sounds/characters/warrior_death.wav");
    assetStore->AddSound(WIZARDDEATH, "./assets/sounds/characters/wizard_death.wav");

    assetStore->AddSound(BUTTON, "./assets/sounds/events/button_click.wav");
    assetStore->AddSound(DEATH, "./assets/sounds/events/death_screen.wav");
    assetStore->AddSound(KILLVOID, "./assets/sounds/events/enter_realm.wav");
    assetStore->AddSound(ERROR, "./assets/sounds/events/error.wav");
    assetStore->AddSound(INVENTORY, "./assets/sounds/events/inventory_move_item.wav");
    assetStore->AddSound(LEVELUP, "./assets/sounds/events/level_up.wav");
    assetStore->AddSound(LOOT, "./assets/sounds/events/loot_appears.wav");
    assetStore->AddSound(NOMANA, "./assets/sounds/events/no_mana.wav");
    assetStore->AddSound(UNLOCKVOID, "./assets/sounds/events/use_key.wav");
    assetStore->AddSound(POTION, "./assets/sounds/events/use_potion.wav");

    assetStore->AddSound(ABYSSBRUTESDEATH, "./assets/sounds/enemies/abyss_brutes_death.wav");
    assetStore->AddSound(ABYSSBRUTESHIT, "./assets/sounds/enemies/abyss_brutes_hit.wav");
    assetStore->AddSound(ABYSSDEMONSDEATH, "./assets/sounds/enemies/abyss_demons_death.wav");
    assetStore->AddSound(ABYSSDEMONSHIT, "./assets/sounds/enemies/abyss_demons_hit.wav");
    assetStore->AddSound(ARCHDEMONDEATH, "./assets/sounds/enemies/archdemon_death.wav");
    assetStore->AddSound(ARCHDEMONHIT, "./assets/sounds/enemies/archdemon_hit.wav");
    assetStore->AddSound(BATSDEATH, "./assets/sounds/enemies/bats_death.wav");
    assetStore->AddSound(BATSHIT, "./assets/sounds/enemies/bats_hit.wav");
    assetStore->AddSound(BEHOLDERDEATH, "./assets/sounds/enemies/beholder_death.wav");
    assetStore->AddSound(BEHOLDERHIT, "./assets/sounds/enemies/beholder_hit.wav");
    assetStore->AddSound(BLOBSDEATH, "./assets/sounds/enemies/blobs_death.wav");
    assetStore->AddSound(BLOBSHIT, "./assets/sounds/enemies/blobs_hit.wav");
    assetStore->AddSound(CAVEPIRATESDEATH, "./assets/sounds/enemies/cave_pirates_death.wav");
    assetStore->AddSound(CAVEPIRATESHIT, "./assets/sounds/enemies/cave_pirates_hit.wav");
    assetStore->AddSound(CHICKENDEATH, "./assets/sounds/enemies/chicken_death.wav");
    assetStore->AddSound(CHICKENHIT, "./assets/sounds/enemies/chicken_hit.wav");
    assetStore->AddSound(CUBESDEATH, "./assets/sounds/enemies/cubes_death.wav");
    assetStore->AddSound(CUBESHIT, "./assets/sounds/enemies/cubes_hit.wav");
    assetStore->AddSound(CYCLOPSDEATH, "./assets/sounds/enemies/cyclops_death.wav");
    assetStore->AddSound(CYCLOPSHIT, "./assets/sounds/enemies/cyclops_hit.wav");
    assetStore->AddSound(DARKELVESDEATH, "./assets/sounds/enemies/dark_elves_death.wav");
    assetStore->AddSound(DARKELVESHIT, "./assets/sounds/enemies/dark_elves_hit.wav");
    assetStore->AddSound(DEFAULTDEATH, "./assets/sounds/enemies/default_death.wav");
    assetStore->AddSound(DEFAULTHIT, "./assets/sounds/enemies/default_hit.wav");
    assetStore->AddSound(DEMONSDEATH, "./assets/sounds/enemies/demons_death.wav");
    assetStore->AddSound(DEMONSHIT, "./assets/sounds/enemies/demons_hit.wav");
    assetStore->AddSound(DJINNDEATH, "./assets/sounds/enemies/djinn_death.wav");
    assetStore->AddSound(DJINNHIT, "./assets/sounds/enemies/djinn_hit.wav");
    assetStore->AddSound(DRAGONSDEATH, "./assets/sounds/enemies/dragons_death.wav");
    assetStore->AddSound(DRAGONSHIT, "./assets/sounds/enemies/dragons_hit.wav");
    assetStore->AddSound(DWARFGODDEATH, "./assets/sounds/enemies/dwarf_god_death.wav");
    assetStore->AddSound(DWARFGODHIT, "./assets/sounds/enemies/dwarf_god_hit.wav");
    assetStore->AddSound(DWARVESDEATH, "./assets/sounds/enemies/dwarves_death.wav");
    assetStore->AddSound(DWARVESHIT, "./assets/sounds/enemies/dwarves_hit.wav");
    assetStore->AddSound(EGGSDEATH, "./assets/sounds/enemies/eggs_death.wav");
    assetStore->AddSound(EGGSHIT, "./assets/sounds/enemies/eggs_hit.wav");
    assetStore->AddSound(ELVESDEATH, "./assets/sounds/enemies/elves_death.wav");
    assetStore->AddSound(ELVESHIT, "./assets/sounds/enemies/elves_hit.wav");
    assetStore->AddSound(ENTSDEATH, "./assets/sounds/enemies/ents_death.wav");
    assetStore->AddSound(ENTSHIT, "./assets/sounds/enemies/ents_hit.wav");
    assetStore->AddSound(FLAMINGSKULLSDEATH, "./assets/sounds/enemies/flaming_skulls_death.wav");
    assetStore->AddSound(FLAMINGSKULLSHIT, "./assets/sounds/enemies/flaming_skulls_hit.wav");
    assetStore->AddSound(FLAYERSDEATH, "./assets/sounds/enemies/flayers_death.wav");
    assetStore->AddSound(FLAYERSHIT, "./assets/sounds/enemies/flayers_hit.wav");
    assetStore->AddSound(FLYINGBRAINDEATH, "./assets/sounds/enemies/flying_brain_death.wav");
    assetStore->AddSound(FLYINGBRAINHIT, "./assets/sounds/enemies/flying_brain_hit.wav");
    assetStore->AddSound(GHOSTSDEATH, "./assets/sounds/enemies/ghosts_death.wav");
    assetStore->AddSound(GHOSTSHIT, "./assets/sounds/enemies/ghosts_hit.wav");
    assetStore->AddSound(GHOSTGODDEATH, "./assets/sounds/enemies/ghost_god_death.wav");
    assetStore->AddSound(GHOSTGODHIT, "./assets/sounds/enemies/ghost_god_hit.wav");
    assetStore->AddSound(GOBLINSDEATH, "./assets/sounds/enemies/goblins_death.wav");
    assetStore->AddSound(GOBLINSHIT, "./assets/sounds/enemies/goblins_hit.wav");
    assetStore->AddSound(GOLEMSDEATH, "./assets/sounds/enemies/golems_death.wav");
    assetStore->AddSound(GOLEMSHIT, "./assets/sounds/enemies/golems_hit.wav");
    assetStore->AddSound(GREATERDEMONSDEATH, "./assets/sounds/enemies/greater_demons_death.wav");
    assetStore->AddSound(GREATERDEMONSHIT, "./assets/sounds/enemies/greater_demons_hit.wav");
    assetStore->AddSound(GREATERPITSNAKESDEATH, "./assets/sounds/enemies/greater_pit_snakes_death.wav");
    assetStore->AddSound(GREATERPITSNAKESHIT, "./assets/sounds/enemies/greater_pit_snakes_hit.wav");
    assetStore->AddSound(HOBBITSDEATH, "./assets/sounds/enemies/hobbits_death.wav");
    assetStore->AddSound(HOBBITSHIT, "./assets/sounds/enemies/hobbits_hit.wav");
    assetStore->AddSound(LIZARDGODDEATH, "./assets/sounds/enemies/lizard_god_death.wav");
    assetStore->AddSound(LIZARDGODHIT, "./assets/sounds/enemies/lizard_god_hit.wav");
    assetStore->AddSound(MEDUSADEATH, "./assets/sounds/enemies/medusa_death.wav");
    assetStore->AddSound(MEDUSAHIT, "./assets/sounds/enemies/medusa_hit.wav");
    assetStore->AddSound(MINIONOFORYXDEATH, "./assets/sounds/enemies/minion_of_oryx_death.wav");
    assetStore->AddSound(MINIONOFORYXHIT, "./assets/sounds/enemies/minion_of_oryx_hit.wav");
    assetStore->AddSound(MUMMIESDEATH, "./assets/sounds/enemies/mummies_death.wav");
    assetStore->AddSound(MUMMIESHIT, "./assets/sounds/enemies/mummies_hit.wav");
    assetStore->AddSound(NIGHTELVESDEATH, "./assets/sounds/enemies/night_elves_death.wav");
    assetStore->AddSound(NIGHTELVESHIT, "./assets/sounds/enemies/night_elves_hit.wav");
    assetStore->AddSound(OGRESDEATH, "./assets/sounds/enemies/ogres_death.wav");
    assetStore->AddSound(OGRESHIT, "./assets/sounds/enemies/ogres_hit.wav");
    assetStore->AddSound(ORCSDEATH, "./assets/sounds/enemies/orcs_death.wav");
    assetStore->AddSound(ORCSHIT, "./assets/sounds/enemies/orcs_hit.wav");
    assetStore->AddSound(ORYXDEATH, "./assets/sounds/enemies/oryx_death.wav");
    assetStore->AddSound(ORYXHIT, "./assets/sounds/enemies/oryx_hit.wav");
    assetStore->AddSound(PIRATESDEATH, "./assets/sounds/enemies/pirates_death.wav");
    assetStore->AddSound(PIRATESHIT, "./assets/sounds/enemies/pirates_hit.wav");
    assetStore->AddSound(PIRATEKINGDEATH, "./assets/sounds/enemies/pirate_king_death.wav");
    assetStore->AddSound(PIRATEKINGHIT, "./assets/sounds/enemies/pirate_king_hit.wav");
    assetStore->AddSound(PITSNAKESDEATH, "./assets/sounds/enemies/pit_snakes_death.wav");
    assetStore->AddSound(PITSNAKESHIT, "./assets/sounds/enemies/pit_snakes_hit.wav");
    assetStore->AddSound(PYTHONSDEATH, "./assets/sounds/enemies/pythons_death.wav");
    assetStore->AddSound(PYTHONSHIT, "./assets/sounds/enemies/pythons_hit.wav");
    assetStore->AddSound(REDKNIGHTDEATH, "./assets/sounds/enemies/red_knight_death.wav");
    assetStore->AddSound(REDKNIGHTHIT, "./assets/sounds/enemies/red_knight_hit.wav");
    assetStore->AddSound(ROCKSDEATH, "./assets/sounds/enemies/rocks_death.wav");
    assetStore->AddSound(ROCKSHIT, "./assets/sounds/enemies/rocks_hit.wav");
    assetStore->AddSound(SCORPIONSDEATH, "./assets/sounds/enemies/scorpions_death.wav");
    assetStore->AddSound(SCORPIONSHIT, "./assets/sounds/enemies/scorpions_hit.wav");
    assetStore->AddSound(SKELETONSDEATH, "./assets/sounds/enemies/skeletons_death.wav");
    assetStore->AddSound(SKELETONSHIT, "./assets/sounds/enemies/skeletons_hit.wav");
    assetStore->AddSound(SKULLSHRINEDEATH, "./assets/sounds/enemies/skull_shrine_death.wav");
    assetStore->AddSound(SKULLSHRINEHIT, "./assets/sounds/enemies/skull_shrine_hit.wav");
    assetStore->AddSound(SLIMESDEATH, "./assets/sounds/enemies/slimes_death.wav");
    assetStore->AddSound(SLIMESHIT, "./assets/sounds/enemies/slimes_hit.wav");
    assetStore->AddSound(SNAKESDEATH, "./assets/sounds/enemies/snakes_death.wav");
    assetStore->AddSound(SNAKESHIT, "./assets/sounds/enemies/snakes_hit.wav");
    assetStore->AddSound(SNAKEQUEENDEATH, "./assets/sounds/enemies/snake_queen_death.wav");
    assetStore->AddSound(SNAKEQUEENHIT, "./assets/sounds/enemies/snake_queen_hit.wav");
    assetStore->AddSound(SPIDERSDEATH, "./assets/sounds/enemies/spiders_death.wav");
    assetStore->AddSound(SPIDERSHIT, "./assets/sounds/enemies/spiders_hit.wav");
    assetStore->AddSound(SPIDERQUEENDEATH, "./assets/sounds/enemies/spider_queen_death.wav");
    assetStore->AddSound(SPIDERQUEENHIT, "./assets/sounds/enemies/spider_queen_hit.wav");
    assetStore->AddSound(SPRITESDEATH, "./assets/sounds/enemies/sprites_death.wav");
    assetStore->AddSound(SPRITESHIT, "./assets/sounds/enemies/sprites_hit.wav");
    assetStore->AddSound(SPRITEGODDEATH, "./assets/sounds/enemies/sprite_god_death.wav");
    assetStore->AddSound(SPRITEGODHIT, "./assets/sounds/enemies/sprite_god_hit.wav");
    assetStore->AddSound(STONEWALLSDEATH, "./assets/sounds/enemies/stone_walls_death.wav");
    assetStore->AddSound(STONEWALLSHIT, "./assets/sounds/enemies/stone_walls_hit.wav");
    assetStore->AddSound(TRAPSDEATH, "./assets/sounds/enemies/traps_death.wav");
    assetStore->AddSound(TRAPSHIT, "./assets/sounds/enemies/traps_hit.wav");
    assetStore->AddSound(TREESDEATH, "./assets/sounds/enemies/trees_death.wav");
    assetStore->AddSound(TREESHIT, "./assets/sounds/enemies/trees_hit.wav");
    assetStore->AddSound(UNDEADDWARVESDEATH, "./assets/sounds/enemies/undead_dwarves_death.wav");
    assetStore->AddSound(UNDEADDWARVESHIT, "./assets/sounds/enemies/undead_dwarves_hit.wav");
    assetStore->AddSound(UNDEADHOBBITSDEATH, "./assets/sounds/enemies/undead_hobbits_death.wav");
    assetStore->AddSound(UNDEADHOBBITSHIT, "./assets/sounds/enemies/undead_hobbits_hit.wav");
    assetStore->AddSound(WOODENWALLSDEATH, "./assets/sounds/enemies/wooden_walls_death.wav");
    assetStore->AddSound(WOODENWALLSHIT, "./assets/sounds/enemies/wooden_walls_hit.wav");

    assetStore->AddFont("damagefont", "./assets/fonts/myriadprosemibold.ttf", 32);
    assetStore->AddFont("namefont","./assets/fonts/myriadprosemibold.ttf", 26);
    assetStore->AddFont("uifont1","./assets/fonts/myriadprosemibold.ttf", 16);
    assetStore->AddFont("statfont", "./assets/fonts/myriadpro.ttf", 15);
    assetStore->AddFont("statfont2", "./assets/fonts/myriadprosemibold.ttf", 15);
    assetStore->AddFont("mpb","./assets/fonts/myriadprobold.ttf", 32);
    
    assetStore->AddMusic("ost", "./assets/sounds/Sorc.ogg");
}

void Game::LoadGui(classes className){

    SDL_Texture * staticHUD =  SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 250, 750);
    SDL_SetTextureBlendMode(staticHUD, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, staticHUD);
    
    SDL_Rect srcRect = {0,0,250,750};
    SDL_Rect dstRect = {0,0,250,750};
    SDL_RenderCopy(renderer, assetStore->GetTexture(GUIBACKGROUND), &srcRect, &dstRect); // HUD background (grey rectangle)
    
    srcRect = {0,className*24,8,8};
    dstRect = {15, 255, static_cast<int>(8*3.5), static_cast<int>(8*3.5)};
    SDL_RenderCopy(renderer, assetStore->GetTexture(PLAYERS), &srcRect, &dstRect); // player icon

    srcRect = {6*16,0,16,16};
    dstRect = {220, 257, static_cast<int>(16*1.3),static_cast<int>(16*1.3)};
    SDL_RenderCopy(renderer, assetStore->GetTexture(LOFIINTERFACEBIG), &srcRect, &dstRect); // nexus button

    SDL_SetRenderDrawColor(renderer,84,84,84,255); // backgrounds for stat bars
    SDL_Rect xpBarBackground = {15, 291, 225, 20};
    SDL_RenderFillRect(renderer, &xpBarBackground);
    SDL_Rect hpBarBackground = {15, 319, 225, 20};
    SDL_RenderFillRect(renderer, &hpBarBackground);
    SDL_Rect mpBarBackground = {15, 347, 225, 20};
    SDL_RenderFillRect(renderer, &mpBarBackground);

    Entity tempMiniMap = registry->CreateEntity();
    tempMiniMap.AddComponent<TransformComponent>(glm::vec2(755,5),glm::vec2(1.0,1.0));
    tempMiniMap.AddComponent<SpriteComponent>(TEMPMINIMAP, 240, 240, 11, 0, 0, true); // mini map (not static!)

    Entity playerName = registry->CreateEntity();
    playerName.AddComponent<TextLabelComponent>("Gabeeeeeee", "namefont", grey, true,0,0,0);
    playerName.AddComponent<TransformComponent>(glm::vec2(800, 257)); // name. could be static, but isn't for now (will do later)

    Entity xpBar = registry->CreateEntity();
    xpBar.AddComponent<DynamicUIEntityComponent>(765,291, 225, 20, 87,117,32);

    Entity hpBar = registry->CreateEntity();
    hpBar.AddComponent<DynamicUIEntityComponent>(765,319, 225, 20, 239,68,46);

    Entity mpBar = registry->CreateEntity();
    mpBar.AddComponent<DynamicUIEntityComponent>(765,347, 225, 20, 95,133,228);

    Entity hpText = registry->CreateEntity();
    hpText.AddComponent<TextLabelComponent>("HP", "uifont1", white, true,0,0,0);
    hpText.AddComponent<TransformComponent>(glm::vec2(767, 323), glm::vec2(1.0,1.0));

    Entity mpText = registry->CreateEntity();
    mpText.AddComponent<TextLabelComponent>("MP", "uifont1", white, true,0,0,0);
    mpText.AddComponent<TransformComponent>(glm::vec2(767, 351), glm::vec2(1.0,1.0));

    const auto& pbs = player.GetComponent<BaseStatComponent>();
    const auto& hpmp = player.GetComponent<HPMPComponent>();
    const auto& offenseStats = player.GetComponent<OffenseStatComponent>();
    const auto& activeSpeedStat = player.GetComponent<SpeedStatComponent>().activespeed;
    const auto& classname = player.GetComponent<ClassNameComponent>().classname;
    SDL_Color color;

    Entity hpDisplayText = registry->CreateEntity();
    hpDisplayText.AddComponent<TextLabelComponent>("", "statfont2", white, true,0,0,0);
    hpDisplayText.GetComponent<TextLabelComponent>().invisible = true;
    hpDisplayText.AddComponent<DisplayStatComponent>();
    hpDisplayText.AddComponent<TransformComponent>(glm::vec2(850,323), glm::vec2(1.0,1.0));

    Entity mpDisplayText = registry->CreateEntity();
    mpDisplayText.AddComponent<TextLabelComponent>("", "statfont2", white, true,0,0,0);
    mpDisplayText.GetComponent<TextLabelComponent>().invisible = true;
    mpDisplayText.AddComponent<DisplayStatComponent>();
    mpDisplayText.AddComponent<TransformComponent>(glm::vec2(850,351), glm::vec2(1.0,1.0));

    Entity attDisplayText = registry->CreateEntity();
    attDisplayText.AddComponent<TextLabelComponent>("", "statfont2", grey, true,0,0,0);
    attDisplayText.AddComponent<DisplayStatComponent>();
    attDisplayText.AddComponent<TransformComponent>(glm::vec2(780+37, 380+3), glm::vec2(1.0,1.0));

    Entity defDisplayText = registry->CreateEntity();
    defDisplayText.AddComponent<TextLabelComponent>("","statfont2", grey, true,0,0,0);
    defDisplayText.AddComponent<DisplayStatComponent>();
    defDisplayText.AddComponent<TransformComponent>(glm::vec2(900+37, 380+3), glm::vec2(1.0,1.0));

    Entity spdDisplayText = registry->CreateEntity();
    spdDisplayText.AddComponent<TextLabelComponent>("","statfont2", grey, true,0,0,0);
    spdDisplayText.AddComponent<DisplayStatComponent>();
    spdDisplayText.AddComponent<TransformComponent>(glm::vec2(780+37,400+3), glm::vec2(1.0,1.0));

    Entity dexDisplayText = registry->CreateEntity();
    dexDisplayText.AddComponent<TextLabelComponent>("","statfont2", grey, true,0,0,0);
    dexDisplayText.AddComponent<DisplayStatComponent>();
    dexDisplayText.AddComponent<TransformComponent>(glm::vec2(900+37,400+3), glm::vec2(1.0,1.0));

    Entity vitDisplayText = registry->CreateEntity();
    vitDisplayText.AddComponent<TextLabelComponent>("","statfont2", grey, true,0,0,0);
    vitDisplayText.AddComponent<DisplayStatComponent>();
    vitDisplayText.AddComponent<TransformComponent>(glm::vec2(780+37,420+3), glm::vec2(1.0,1.0));

    Entity wisDisplayText = registry->CreateEntity();
    wisDisplayText.AddComponent<TextLabelComponent>("","statfont2", grey, true,0,0,0);
    wisDisplayText.AddComponent<DisplayStatComponent>();
    wisDisplayText.AddComponent<TransformComponent>(glm::vec2(900+37,420+3), glm::vec2(1.0,1.0));

    Entity xpDisplayText = registry->CreateEntity();
    xpDisplayText.AddComponent<TextLabelComponent>("", "statfont2", white, true,0,0,0);
    xpDisplayText.GetComponent<TextLabelComponent>().invisible = true;
    xpDisplayText.AddComponent<DisplayStatComponent>();
    xpDisplayText.AddComponent<TransformComponent>(glm::vec2(850, 295), glm::vec2(1.0,1.0));

    //xp text hard-coded at position 9 in UpdateDisplayStatTextSystem.entities
    Entity xpText = registry->CreateEntity();
    xpText.AddComponent<TextLabelComponent>("Lvl " + std::to_string(static_cast<int>(pbs.level)) , "uifont1", white, true,0,0,0);
    xpText.AddComponent<TransformComponent>(glm::vec2(767, 295), glm::vec2(1.0,1.0));
    xpText.AddComponent<DisplayStatComponent>();

    int width; 
    int height;
    SDL_Surface* ttfSurface;
    SDL_Texture* ttfTextureFromSurface;

    ttfSurface = TTF_RenderText_Blended(assetStore->GetFont("statfont"), "ATT -", grey);
    ttfTextureFromSurface = SDL_CreateTextureFromSurface(renderer, ttfSurface);
    SDL_QueryTexture(ttfTextureFromSurface, NULL, NULL, &width, &height);
    dstRect = {32,380, width, height};
    SDL_RenderCopy(renderer, ttfTextureFromSurface, NULL, &dstRect);

    ttfSurface = TTF_RenderText_Blended(assetStore->GetFont("statfont"), "DEF -", grey);
    ttfTextureFromSurface = SDL_CreateTextureFromSurface(renderer, ttfSurface);
    SDL_QueryTexture(ttfTextureFromSurface, NULL, NULL, &width, &height);
    dstRect = {150,380, width, height};
    SDL_RenderCopy(renderer, ttfTextureFromSurface, NULL, &dstRect);

    ttfSurface = TTF_RenderText_Blended(assetStore->GetFont("statfont"), "SPD -", grey);
    ttfTextureFromSurface = SDL_CreateTextureFromSurface(renderer, ttfSurface);
    SDL_QueryTexture(ttfTextureFromSurface, NULL, NULL, &width, &height);
    dstRect = {30,400, width, height};
    SDL_RenderCopy(renderer, ttfTextureFromSurface, NULL, &dstRect);

    ttfSurface = TTF_RenderText_Blended(assetStore->GetFont("statfont"), "DEX -", grey);
    ttfTextureFromSurface = SDL_CreateTextureFromSurface(renderer, ttfSurface);
    SDL_QueryTexture(ttfTextureFromSurface, NULL, NULL, &width, &height);
    dstRect = {150,400, width, height};
    SDL_RenderCopy(renderer, ttfTextureFromSurface, NULL, &dstRect);

    ttfSurface = TTF_RenderText_Blended(assetStore->GetFont("statfont"), "VIT -", grey);
    ttfTextureFromSurface = SDL_CreateTextureFromSurface(renderer, ttfSurface);
    SDL_QueryTexture(ttfTextureFromSurface, NULL, NULL, &width, &height);
    dstRect = {36,420, width, height};
    SDL_RenderCopy(renderer, ttfTextureFromSurface, NULL, &dstRect);

    ttfSurface = TTF_RenderText_Blended(assetStore->GetFont("statfont"), "WIS -", grey);
    ttfTextureFromSurface = SDL_CreateTextureFromSurface(renderer, ttfSurface);
    SDL_QueryTexture(ttfTextureFromSurface, NULL, NULL, &width, &height);
    dstRect = {150,420, width, height};
    SDL_RenderCopy(renderer, ttfTextureFromSurface, NULL, &dstRect);

    Entity weaponSlot = registry->CreateEntity();
    weaponSlot.AddComponent<TransformComponent>(glm::vec2(765+5, 450+5), glm::vec2(1.25,1.25)); 
    if(classname == WARRIOR){
        weaponSlot.AddComponent<SpriteComponent>(INVENTORYICONS, 44-8, 44-8, 11, 44*6+4, 0+4, true);
    } else if (classname == PRIEST){
        weaponSlot.AddComponent<SpriteComponent>(INVENTORYICONS, 44-8, 44-8, 11, 44*8+4, 0+4, true);
    } else if (classname == ARCHER){
        weaponSlot.AddComponent<SpriteComponent>(INVENTORYICONS, 44-8, 44-8, 11, 44*1+4, 0+4, true);
    }
    equipmentIconIds.push_back(weaponSlot.GetId());

    Entity abilitySlot = registry->CreateEntity();
    abilitySlot.AddComponent<TransformComponent>(glm::vec2(765+44+12+1 +5, 450+5), glm::vec2(1.25,1.25));
    if(classname == WARRIOR){
        abilitySlot.AddComponent<SpriteComponent>(INVENTORYICONS, 44-8, 44-8, 11, 44*0+4, 0+4, true);
    } else if (classname == ARCHER){
        abilitySlot.AddComponent<SpriteComponent>(INVENTORYICONS, 44-8, 44-8, 11, 44*0+4, 44*1+4, true);
    } else if (classname == PRIEST){
        abilitySlot.AddComponent<SpriteComponent>(INVENTORYICONS, 44-8, 44-8, 11, 44*7+4, 0+4, true);
    }
    equipmentIconIds.push_back(abilitySlot.GetId());

    Entity armorSlot = registry->CreateEntity();
    armorSlot.AddComponent<TransformComponent>(glm::vec2(765+44*2+12*2+1 +5, 450+5), glm::vec2(1.25,1.25));
    if(classname == WARRIOR){
        armorSlot.AddComponent<SpriteComponent>(INVENTORYICONS, 44-8, 44-8, 11, 44*2+4, 0+4, true);
    } else if (classname == ARCHER){
        armorSlot.AddComponent<SpriteComponent>(INVENTORYICONS, 44-8, 44-8, 11, 44*3+4, 0+4, true);
    } else if (classname == PRIEST){
        armorSlot.AddComponent<SpriteComponent>(INVENTORYICONS,44-8, 44-8, 11, 44*5+4, 0+4, true);
    }
    equipmentIconIds.push_back(armorSlot.GetId());

    Entity ringSlot = registry->CreateEntity();
    ringSlot.AddComponent<TransformComponent>(glm::vec2(765+44*3+12*3+1 +5, 450+5), glm::vec2(1.25,1.25));
    ringSlot.AddComponent<SpriteComponent>(INVENTORYICONS, 44-8, 44-8, 11, 44*4+4, 0+4, true);
    equipmentIconIds.push_back(ringSlot.GetId());

    int invSlotDimension = static_cast<int>(44 * 1.25);
    srcRect = {44*9, 0, 44, 44};

    dstRect = {15, 450, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // weapon slot bg
    dstRect = {(765+44+12+1)-750, 450, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // ability slot bg
    dstRect = {(765+44*2+12*2+1)-750, 450, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // armor slot bg
    dstRect = {(765+44*3+12*3+1)-750, 450, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // ring slot bg

    dstRect = {(765)-750, 450+44+12+1, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // slot one bg
    dstRect = {(765+44*1+12*1+1)-750, 450+44+12+1, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // slot two bg
    dstRect = {(765+44*2+12*2+1)-750, 450+44+12+1, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // slot three bg
    dstRect = {(765+44*3+12*3+1)-750, 450+44+12+1, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // slot four bg
    dstRect = {(765)-750, 450+((44+12+1)*2), invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // slot five bg
    dstRect = {(765+44*1+12*1+1)-750, 450+((44+12+1)*2), invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // slot six bg
    dstRect = {(765+44*2+12*2+1)-750, 450+((44+12+1)*2), invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // slot seven bg
    dstRect = {(765+44*3+12*3+1)-750, 450+((44+12+1)*2), invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // slot eight bg

    //todo make inventory icons (numbers 1-8) & push back to the vector 

    assetStore->AddTexture(renderer, STATICHUD, staticHUD);
    Entity statichud = registry->CreateEntity();
    statichud.AddComponent<SpriteComponent>(STATICHUD, 250, 750, 10, 0, 0 , true);
    statichud.AddComponent<TransformComponent>(glm::vec2(750,0), glm::vec2(1.0,1.0));
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderClear(renderer);

    /* inventory slots are their own texture */
    SDL_Texture * bagslots =  SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 250, 750);
    SDL_SetTextureBlendMode(bagslots, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, bagslots);
    dstRect = {765-750, 631, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // bag slot one bg
    dstRect = {822-750, 631, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // bag slot two bg
    dstRect = {878-750, 631, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // bag slot three bg
    dstRect = {934-750, 631, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // bag slot four bg
    dstRect = {765-750, 688, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // bag slot five bg
    dstRect = {822-750, 688, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // bag slot six bg
    dstRect = {878-750, 688, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // bag slot seven bg
    dstRect = {934-750, 688, invSlotDimension, invSlotDimension};
    SDL_RenderCopy(renderer, assetStore->GetTexture(INVENTORYICONS), &srcRect, &dstRect); // bag slot eight bg
    assetStore->AddTexture(renderer, BAGSLOTS, bagslots);
    Entity bagSlots = registry->CreateEntity();
    bagSlots.AddComponent<SpriteComponent>(BAGSLOTS, 250, 750, 9, 0, 0, true);
    bagSlots.AddComponent<TransformComponent>(glm::vec2(750,0), glm::vec2(1.0,1.0));
    bagSlots.AddComponent<InteractUIComponent>();
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderClear(renderer);

    SDL_FreeSurface(ttfSurface);
}

void Game::LoadEnemy(glm::vec2 spawnpoint, sprites spriteEnum){
    factory->spawnMonster(registry, spawnpoint, spriteEnum);
}

void Game::LoadPlayer(classes classname){
    player = registry->CreateEntity();
    player.AddComponent<TransformComponent>(playerSpawn); //scaled by 6, an 8x8 sprite is effectively 48x48 pixels
    player.AddComponent<SpriteComponent>(classname);
    player.AddComponent<ClassNameComponent>(classname); 
    player.AddComponent<BaseStatComponent>(classname);
    const auto& baseStats = player.GetComponent<BaseStatComponent>();
    player.AddComponent<HPMPComponent>(baseStats);
    player.AddComponent<OffenseStatComponent>(baseStats);
    player.AddComponent<SpeedStatComponent>(baseStats);
    player.AddComponent<RidigBodyComponent>();
    player.AddComponent<AnimationComponent>(2,50, false);
    player.AddComponent<KeyboardControlledComponent>();
    player.AddComponent<BoxColliderComponent>(STANDARD);
    player.AddComponent<CameraFollowComponent>();
    player.AddComponent<CollisionFlagComponent>();
    player.Group(PLAYER);
    player.AddComponent<PlayerItemsComponent>();
    player.AddComponent<AnimatedShootingComponent>(classname);
    player.AddComponent<StatusEffectComponent>();

    player.GetComponent<HPMPComponent>().activemp = 32000;
    player.GetComponent<HPMPComponent>().maxmp = 32000;
    player.GetComponent<OffenseStatComponent>().activeattack = 74;
    player.GetComponent<BaseStatComponent>().attack = 74;
    player.GetComponent<SpeedStatComponent>().activespeed = 50;
    player.GetComponent<BaseStatComponent>().speed = 50;

    player.AddComponent<ProjectileEmitterComponent>();
    player.GetComponent<ProjectileEmitterComponent>().repeatFrequency = 1000 / (.08666 * baseStats.dexterity + 1.5);

    player.AddComponent<AbilityComponent>();
    auto& ac = player.GetComponent<AbilityComponent>();
    ac.coolDownMS = 500;
    ac.mpRequired = 20;
    switch(classname){
        case ARCHER:{
            player.AddComponent<QuiverComponent>();
            break;
        }
        case PRIEST: {
            player.AddComponent<TomeComponent>();
            break;
        }
        case WARRIOR: {
            player.AddComponent<HelmComponent>();
            break;
        }
    }
}

void Game::PopulateRegistry(){
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<KeyboardMovementSystem>();
    registry->AddSystem<RenderColliderSystem>();
    registry->AddSystem<CameraMovementSystem>();
    registry->AddSystem<ProjectileEmitSystem>();
    registry->AddSystem<ProjectileLifeCycleSystem>();
    registry->AddSystem<RenderTextSystem>();
    registry->AddSystem<StatSystem>();
    registry->AddSystem<DynamicUIRenderSystem>();
    registry->AddSystem<UpdateDisplayStatTextSystem>();
    registry->AddSystem<ProjectileMovementSystem>();
    registry->AddSystem<PassiveAISystem>();
    registry->AddSystem<ChaseAISystem>();
    registry->AddSystem<NeutralAISystem>();
    registry->AddSystem<TrapAISystem>();
    registry->AddSystem<AnimatedChaseAISystem>();
    registry->AddSystem<AnimatedNeutralAISystem>();
    registry->AddSystem<LootBagSystem>();
    registry->AddSystem<InteractUISystem>();
    registry->AddSystem<ItemMovementSystem>();
    registry->AddSystem<AbilitySystem>();
    registry->AddSystem<StatusEffectSystem>();
    if(debug){
        registry->AddSystem<RenderMouseBoxSystem>();
        registry->AddSystem<RenderColliderSystem>();
    }
    
}

void Game::LoadLevel(int level){
    
}

void Game::Setup(){ // everything in setup occurs before game loop begins
    PopulateAssetStore();
    PopulateRegistry();
    LoadPlayer(ARCHER); // MUST LOAD PLAYER FIRST SO THEY HAVE ENTITYID OF 0!
    const auto& playerClassName = player.GetComponent<ClassNameComponent>().classname;
    LoadGui(playerClassName);
    registry->Update();
    registry->GetSystem<UpdateDisplayStatTextSystem>().SubscribeToEvents(eventBus);
    eventBus->EmitEvent<UpdateDisplayStatEvent>(player);
    LoadTileMap(UDL, "./assets/tilemaps/wallTest.map");

}

void Game::Update(){
    // SDL_GetTicks counts ms passed since SDL_Init was called in Initialize(); 
    if (fpslimit) {
        int timeToWait = MILLISECONDS_PER_FRAME - SDL_GetTicks() - millisecsPreviousFrame; 
        if (timeToWait > 0 && timeToWait <= MILLISECONDS_PER_FRAME){
            SDL_Delay(timeToWait); //doesn't burn clock cycles; returns resources to OS for timeToWait
        }
    }
    // difference in ticks since last frame in seconds
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
    millisecsPreviousFrame = SDL_GetTicks(); // every ms is a tick

    //reset all event handlers; reset subscribers 
    eventBus->Reset();

    // SUBCRIPTION IS FRAME-BY-FRAME BINDING. ; do we want to subscribe this frame?
    // later may make it not frame-by-frame and make it static
    registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus); 
    registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<StatSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<UpdateDisplayStatTextSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<InteractUISystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<AbilitySystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<StatusEffectSystem>().SubscribeToEvents(eventBus);
    
    // update registry to process entities that are awaitng creation/deletion and add them to system vectors
    registry->Update();

    // TODO PASS BY CONSTANT REFERENCE INSTANCE OF COPY WHERE APPROPRIATE
    const auto& playerpos = player.GetComponent<TransformComponent>().position;
    auto& playerInventory = player.GetComponent<PlayerItemsComponent>();
    registry->GetSystem<KeyboardMovementSystem>().Update(keysPressed, Game::mouseX, Game::mouseY, camera, space, assetStore, eventBus, registry);
    registry->GetSystem<PassiveAISystem>().Update(playerpos, assetStore);
    registry->GetSystem<ChaseAISystem>().Update(playerpos, assetStore);
    registry->GetSystem<NeutralAISystem>().Update(playerpos, assetStore);
    registry->GetSystem<TrapAISystem>().Update(playerpos, assetStore);
    registry->GetSystem<AnimatedChaseAISystem>().Update(playerpos, assetStore);
    registry->GetSystem<AnimatedNeutralAISystem>().Update(playerpos, assetStore);
    registry->GetSystem<MovementSystem>().Update(deltaTime, registry);
    registry->GetSystem<ProjectileMovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update(camera);
    registry->GetSystem<CollisionSystem>().Update(eventBus, registry, assetStore, deltaTime, playerInventory, factory);
    registry->GetSystem<CameraMovementSystem>().Update(camera);
    registry->GetSystem<ProjectileEmitSystem>().Update(registry, camera, Game::mouseX, Game::mouseY, playerpos, assetStore);
    registry->GetSystem<ProjectileLifeCycleSystem>().Update();
    registry->GetSystem<DamageSystem>().Update(deltaTime, player);
    registry->GetSystem<UpdateDisplayStatTextSystem>().Update(Game::mouseX, Game::mouseY, player, assetStore, renderer);
    registry->GetSystem<ItemMovementSystem>().Update(Game::mouseX, Game::mouseY, keysPressed[4], assetStore, registry, eventBus, player, inventoryIconIds, equipmentIconIds, factory, shift);
    registry->GetSystem<LootBagSystem>().Update(Game::mouseY, player, eventBus, assetStore, registry, playerInventory);
    registry->GetSystem<StatusEffectSystem>().Update(eventBus);
}

void Game::Render(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
    SDL_RenderClear(renderer);
    registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
    registry->GetSystem<DynamicUIRenderSystem>().Update(renderer, player);
    registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera, registry);
    if(debug){
        registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);
        registry->GetSystem<RenderMouseBoxSystem>().Update(renderer, camera);
    }
    SDL_RenderPresent(renderer); 
}

void Game::Destory(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    registry->GetSystem<RenderTextSystem>().killTextures();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}