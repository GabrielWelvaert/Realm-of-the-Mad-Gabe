#include "Game/Game.h"
int main(int argc, char* argv[]){
    Game game;
    game.Initialize();
    game.Run(true);
    game.Destory();
    return 0;
}
