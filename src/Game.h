#ifndef GAME_H
#define GAME_H

#include "Application.h"

#include "Components/LevelGeometry.h"

#include "Graphics/Font.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "Graphics/ShaderProgram2D.h"
#include "Graphics/ShaderProgram3D.h"
#include "Graphics/include/ColourRGBA.h"
#include "Graphics/include/FontRect.h"

using namespace wind;

class Player;
class Wall;
class Block;

namespace
{
    //This holds the number of bodies for the vector of unique_ptrs.
    const unsigned int static NUM_OF_CUBES = 1;
    const unsigned int static NUM_OF_PLANES = 2;
    const unsigned int static NUM_OF_COMPONENTS = 3;

    const int JOYSTICK_DEAD_ZONE = 3200;
};

namespace wind
{
class Game : public RigidBodyApplication
{
public:
    Game();
    ~Game();

    void mainLoop();
private:
    void loadPrograms();
    void loadMedia();
    void loadMeshes();
    void gameLogic();
    void resetGame();
    void handleEvents();
    void update();
    void Display();

    virtual void generateContacts() final;
    virtual void updateObjects(wind::real duration) final;
    virtual void reset() final;

    //This is the mesh register for all the different meshes in the game.
    MeshReg reg;
    //These are the physical components of the games.
    std::vector<std::shared_ptr<Block>> objects;
    std::vector<std::shared_ptr<Wall>> planes;
    std::shared_ptr<Player> player1;
    //The instance shader is for binding and passing everything to the shaders.
    ShaderProgram3D scene;
    //The texture handles the texture, can be binded to other objects.
    Texture texture;
    Texture blockTexture;
    Texture fontTexture;

    std::stringstream TimerString;

    //The quaternion hands rotation and the camera instance holds handles linear movement and
    Quaternion cameraRot;

    //The trans instance holds all the different transforms across the game.
    std::vector<std::unique_ptr<RigidBody>> trans;
    std::vector<std::unique_ptr<Mesh>> mesh;
    Random rand;
    Timer PhysicsClock;
    Timer countDown;
    //Shader shad;

    /*For rendering text**/
    ShaderProgram2D fontProgram2D;
    ShaderProgram2D fontTimer2D;
    Font font;
    ColourRGBA textColour;
    ColourRGBA levelColour;
    FontRect screenRect;

    std::string tester;
    std::string time;
    wind::real mousevel;

    int MidX;
    int MidY;

    unsigned int textPosition;

    int timeLeft;

    int blockCount;

    bool running;
    bool runOnce;
    bool timerFlag;
    bool gameOver;
};
}; //wind
#endif
