#ifndef GAME_H
#define GAME_H
#include "../../libs/SDL2/SDL.h"
#include "../../libs/SDL2/SDL_image.h"
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"
#include "../../libs/glm/glm.hpp"
#include "../Utils/Xoshiro256.h"
#include "../Utils/enums.h"
#include "../Utils/factory.h"
#include "../Utils/room.h"
#include "../Utils/CharacterManager.h"
#include "../Utils/deadPlayer.h"
#include <string>
#include "../Utils/roomShut.h"

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
        const bool debug = false;
        Entity player;
        Xoshiro256 RNG;
        std::vector<int> inventoryIconIds;
        std::vector<int> equipmentIconIds;
        std::string activeCharacterID = "-1"; // ID for saving character; -1 means not selected, 0 means dead character
        std::vector<room> dungeonRooms;
        int bossRoomId;
        room bossRoom;
        bool successfulMapGen = false; 
        deadPlayer deadPlayer = {WIZARD, -1, NONESPRITE, 0};
        roomShut roomShut;

    public:
        Game();
        ~Game();
        void Initialize();
        void Run(bool populate);
        void Setup(bool populate, bool mainmenus, wallTheme area);
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
        void PopulatePlayerInventoryAndEquipment();
        void PopulateEventBus();
        void MainMenus();
        void Background();
        void PopulateItemIconsInAssetStore();
        void SpawnAreaEntities(wallTheme area);
        bool GenerateMap(const wallTheme& wallTheme, std::vector<std::vector<int>>& map);
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
        std::bitset<5> keysPressed;
};

#endif