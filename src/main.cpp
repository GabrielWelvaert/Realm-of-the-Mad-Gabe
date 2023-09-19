#include <iostream>
#include "Game/Game.h"
#include "../libs/SDL2/SDL.h"
#include "../libs/SDL2/SDL_image.h"
#include "../libs/SDL2/SDL_ttf.h"
#include "../libs/SDL2/SDL_mixer.h"
#include "../libs/glm/glm.hpp"
#include "../libs/imgui/imgui.h"
#include "../libs/sol/sol.hpp"
#include "../libs/lua/lua.hpp"

int main(int argc, char* argv[]){

    Game game;
    game.Initialize();
    game.Run();
    game.Destory();


    return 0;
}
