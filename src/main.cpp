#include "Game.h"
/**
    Update the player so the player has a camera.
*/
int main(int argv, char *argc[])
{
    std::unique_ptr<Game> game(std::make_unique<Game>());
    game->mainLoop();

    return 0;
}
