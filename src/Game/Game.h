#ifndef GAME_H
#define GAME_H
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"
#include "../Utils/Xoshiro256.h"
#include "../Utils/enums.h"
#include "../Utils/room.h"
#include "../Utils/CharacterManager.h"
#include "../Utils/DeadPlayer.h"
#include <string>
#include "../Utils/RoomShut.h"
#include "../Utils/vechash.h"
#include "../Utils/BossIds.h"
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
#include "../Systems/ItemIconSystem.h"
#include "../Systems/VaultSystem.h"
#include "../Systems/PortalSystem.h"
#include "../Systems/DisplayNameSystem.h"
#include "../Utils/room.h"
#include <queue>
#include <ctime>
#include "../Utils/RoomShut.h"
#include "../Systems/VaultItemKillSystem.h"
#include "../Systems/OscillatingProjectileMovementSystem.h"
#include "../Systems/SecondaryProjectileEmitSystem.h"
#include "../Systems/RotationSystem.h"
#include "../Systems/RenderMiniMapSystem.h"
#include "../Systems/DistanceToPlayerSystem.h"
#include "../Systems/ParabolicMovementSystem.h"
#include "../Systems/ParticleSystem.h"
#include "../Systems/EnemySpawnSystem.h"
#include "../Systems/MinionSpawnSystem.h"
#include "../Systems/OrbitalMovementSystem.h"
#include "../Utils/KeyBoardInput.h"
#include "../Systems/DeathActionSystem.h"
#include <cstring>
#include <utility>
#include "../Systems/HealOtherSystem.h"
#include "../Systems/BoomerangSystem.h"

const int FPS = 60;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game{
    private:
        bool isRunning;
        int millisecsPreviousFrame = 0;
        SDL_Window* window;
        SDL_Renderer* renderer; 
        SDL_Surface* icon;
        SDL_Rect camera;
        std::unique_ptr<AssetStore> assetStore;
        std::unique_ptr<Registry> registry; 
        std::unique_ptr<EventBus> eventBus;
        std::unique_ptr<Factory> factory;
        std::unique_ptr<CharacterManager> characterManager;
        std::unique_ptr<KeyBoardInput> keyboardinput;
        const bool debug = false;
        Entity player;
        Xoshiro256 RNG;
        std::vector<int> inventoryIconIds;
        std::vector<int> equipmentIconIds;
        std::string activeCharacterID = "-1"; // ID for saving character; -1 means not selected, 0 means dead character
        std::vector<room> dungeonRooms;
        int bossRoomId;
        room bossRoom;
        std::map<cardinalDirection, room> cardinalRooms; // used for the abyss dungeon
        bool successfulMapGen = false; 
        DeadPlayer deadPlayer = {WIZARD, -1, NONESPRITE, 0};
        RoomShut roomShut;
        std::vector<BossIds> bosses;
        int idOfMiniMapEntity = -1;
        int idOfMiniMapVeil;
        int rendererMaxTextureDimension;
        Entity minimapfloor;
        SDL_Texture * mapveilptr = nullptr;        

    public:
        Game();
        void Initialize();
        void Run(bool populate);
        void Setup(bool populate, bool mainmenus, wallTheme area, bool changeChar = false);
        void ProcessInput();
        void Update();
        void Render();
        void Destory();
        void LoadLevel(int level);
        void PopulateAssetStore();
        void LoadPlayer();
        void LoadGui();
        void LoadTileMap(const wallTheme& wallTheme);
        void PopulateRegistry();
        void PopulatePlayerInventoryAndEquipment(const wallTheme& wallTheme);
        void PopulateEventBus();
        void MainMenus(bool changeChar = false);
        void Background();
        void PopulateItemIconsInAssetStore();
        void SpawnAreaEntities(wallTheme area);
        bool GenerateMap(const wallTheme& wallTheme, std::vector<std::vector<int>>& map);
        void MiniMap(const wallTheme& wallTheme, std::vector<std::vector<int>>& map);
        std::vector<Entity> loadMenuOne();
        std::vector<Entity> loadMenuTwo(int numcharacters);
        std::vector<Entity> loadMenuThree();
        std::vector<Entity> loadDeathMenu();
        double deltaTime;
       
        static int windowWidth;
        static int windowHeight;
        static int mapWidth;
        static int mapheight;
        static int mouseX;
        static int mouseY;
        glm::vec2 playerSpawn = glm::vec2(750, 1575);
        wallTheme currentArea = NEXUS;
};

#endif