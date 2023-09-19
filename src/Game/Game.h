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
const bool fpslimit = false;
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
        const bool debug = false;
        Entity player;
        Xoshiro256 RNG;

    public:
        Game();
        ~Game();
        void Initialize();
        void Run();
        void Setup();
        void ProcessInput();
        void Update();
        void Render();
        void Destory();
        void LoadLevel(int level);
        void LoadTileMap(const textureEnums& name, const std::string& tilemap, const std::string& map, int tilesize, double tileScale, std::vector<glm::ivec2> walls);
        void PopulateAssetStore();
        void LoadPlayer();
        void LoadGui(classes className);
        void LoadEnemy(glm::vec2 spawnpoint, sprites spriteEnum);
        void LoadTileMap(const wallTheme& wallTheme, const std::string& pathToMapFile);
       
        static int windowWidth;
        static int windowHeight;
        static int mapWidth;
        static int mapheight;
        static int mouseX;
        static int mouseY;
        glm::vec2 playerSpawn = glm::vec2(750/2 - 20, 750/2 - 20);
};

#endif