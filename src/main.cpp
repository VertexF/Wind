//#include "Game.h"
#include "SDL.h"
#include "GL/glew.h"
#include "il/il.h"
/**
    Update the player so the player has a camera.
*/
int main(int argv, char** argc)
{
    //Game game;
    //game.mainLoop();

    SDL_Init(SDL_INIT_EVERYTHING);
    glewInit();
    ilInit();

    return 0;
}
