#include "Game.h"

#include "Components/Block.h"
#include "Components/Plane.h"
#include "Components/Player.h"

#include "Graphics/ShaderProgram3D.h"
namespace wind
{
Game::Game() : RigidBodyApplication("Cube Game", 800, 600),
    cameraRot(0.0, 0.0, 0.0, 0.0),
    running(true),
    runOnce(false),
    player1(std::make_shared<Player>(_ratio))
{
    textColour.r = 1.0f;
    textColour.g = 0.5f;
    textColour.b = 1.0f;
    textColour.a = 0.5f;

    screenRect.x = 0.0f;
    screenRect.y = 0.0f;
    screenRect.w = static_cast<GLfloat>(_width);
    screenRect.h = static_cast<GLfloat>(_height);

    levelColour = { 0.7, 0.7, 0.7, 1.0 };

    textPosition = FONT_TEXT_ALIGN_CENTERED_H;

    //Important when adding another cude make sure that you increment this number in the for loop.
    for (unsigned int i = 0; i < NUM_OF_CUBES; i++)
    {
        objects.emplace_back(std::make_shared<Block>());
    }

    for (unsigned int i = 0; i < NUM_OF_PLANES; i++)
    {
        planes.emplace_back(std::make_shared<Wall>());
    }

    //Reset need to be called before meshes are created
    reset();
    loadPrograms();
    loadMedia();
    loadMeshes();

    timeLeft = 100000;
    MidX = _width / 2;
    MidY = _height / 2;
    mousevel = 0.1;
    SDL_ShowCursor(SDL_DISABLE);

    PhysicsClock.start();
    //countDown.Start();

    timerFlag = false;
    gameOver = false;
    blockCount = 0;
    glEnable(GL_TEXTURE_2D);

    _seconds = 30.0;
    _timeLeft = 30;
}

Game::~Game()
{
    SDL_Quit();
}

void Game::loadPrograms()
{

    if (!fontProgram2D.loadProgram())
    {
        std::cerr << "Unable to load font rendering program!" << std::endl;
    }

    //Bind font shader program
    fontProgram2D.bind();

    //Initialize projection
    fontProgram2D.setProjection(Matrix4x4().orthoRH(0.0, _width, _height, 0.0, 1.0, -1.0));
    fontProgram2D.updateProjection();

    //Initialize modelview
    fontProgram2D.setModelView(Matrix4x4());
    fontProgram2D.updateModelView();

    fontProgram2D.unbind();

    if (!fontTimer2D.loadProgram())
    {
        std::cerr << "Unable to load font rendering program!" << std::endl;
    }

    //Bind font shader program
    fontTimer2D.bind();

    //Initialize projection
    fontTimer2D.setProjection(Matrix4x4().orthoRH(0.0, _width, _height, 0.0, 1.0, -1.0));
    fontTimer2D.updateProjection();

    //Initialize modelview
    fontTimer2D.setModelView(Matrix4x4());
    fontTimer2D.updateModelView();

    //Set texture unit
    //fontTimer2D.setTextureUnit(0);

    fontTimer2D.unbind();

    if (!scene.loadProgram())
    {
        std::cerr << "Unable to load scene rendering program!" << std::endl;
    }

    scene.bind();

    //scene.setTextureUnit(0);

    scene.unbind();
}

void Game::loadMeshes()
{
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        objects.at(i)->loadMesh("res/cube3.obj");
        reg.add(objects.at(i)->getMesh());
    }

    for (unsigned int i = 0; i < planes.size(); i++)
    {
        planes.at(i)->loadMesh("res/plane.obj");
        reg.add(planes.at(i)->getMesh());
    }

    player1->loadMesh("res/cube3.obj");
    reg.add(player1->getMesh());
}

void Game::loadMedia()
{
    if (!blockTexture.loadTextureFromFile32("res/bricks.jpg"))
    {
        std::cerr << "Unable to load block Texture !" << std::endl;
    }

    if (!texture.loadTextureFromFile32("res/ground.jpg"))
    {
        std::cerr << "Unable to load texture!" << std::endl;
    }

    if (!font.loadImage("res/lazy_font.png"))
    {
        std::cerr << "Unable to load font!" << std::endl;
    }
}

void Game::resetGame()
{
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        objects.at(i)->setState(rand.RandomXZVector(50.0), wind::Vector3(2.0, 2.0, 2.0));
    }

    player1->changePosition(wind::Vector3(0.0, 0.0, 0.0));

    blockCount = 0;
    tester = "Find the blocks!";
    timerFlag = false;
    textPosition = FONT_TEXT_ALIGN_CENTERED_H;
    textColour.r = 1.0f;
    textColour.g = 0.5f;
    textColour.b = 1.0f;
    textColour.a = 0.5f;
    _timeLeft = 30;

    gameOver = false;

}

void Game::gameLogic()
{
    if (_timeLeft > 0)
    {
        _timeLeft = static_cast<int>(_seconds - (countDown.getTicks() / 1000));
        TimerString.str("");
        TimerString << "Time : " << _timeLeft;

        if (blockCount > 0)
        {
            tester = "Total blocks: " + std::to_string(blockCount);
        }
    }
    else
    {
        TimerString.str("");
        countDown.stop();

        textPosition = FONT_TEXT_ALIGN_CENTERED_H | FONT_TEXT_ALIGN_CENTERED_V;
        textColour.r = 0.4f;
        textColour.g = 0.6f;
        textColour.b = 0.8f;
        textColour.a = 1.0f;
        tester = "You got " + std::to_string(blockCount) + " Blocks! \n Press r to play again";

        gameOver = true;
    }
}

void Game::generateContacts()
{
    //Next we need contact data next
    _collData.reset(MAX_CONTACTS);
    _collData.friction = 0.9;
    _collData.tolerance = 0.1;
    _collData.restitution = 0.1;

    if (!objects.empty())
    {
        for (unsigned int i = 0; i < objects.size(); i++)
        {
            if (!_collData.anyContactsLeft())
            {
                return;
            }

            wind::CollisionDetection::BoxAndHalfSpace(*objects.at(i), *planes.at(0), _collData);

            if (!gameOver)
            {
                if (wind::IntersectionTests::BoxAndBox(*player1, *objects.at(i)))
                {
                    //delete objects.at(i);
                    //objects.erase(objects.begin() + i);
                    //for(unsigned int i = 0; i < objects.size(); i++)
                    //{
                    objects.at(0)->setState(rand.RandomXZVector(50.0), wind::Vector3(2.0, 2.0, 2.0));
                    //}

                    blockCount++;
                }
            }
            //wind::PlayerGeometry::BoxAndBox(*player1, *objects.at(i), &collData);
            //wind::CollisionDetection::BoxAndHalfSpace(*player1, *planes.at(1), collData);
            //wind::CollisionDetection::BoxAndHalfSpace(*player1[0], *planes.at(1), collData);

        }
    }
}

void Game::updateObjects(wind::real duration)
{
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        objects.at(i)->update(duration);
    }

    planes.at(0)->update(duration);
    planes.at(1)->update(duration);
    player1->update(duration);
}

void Game::reset()
{
    //Note: The planes arguements are for position and size.
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        objects.at(i)->setState(rand.RandomXZVector(50.0), wind::Vector3(2.0, 2.0, 2.0));
    }

    //Note: The planes arguements are for position and direction.
    planes.at(0)->setState(wind::Vector3(0.0, -5.0, 0.0), wind::Vector3(0.0, 1.0, 0.0));
    //planes.at(0)->initRotation(wind::Vector3::X, wind::Vector3(0.0, -5.0, 0.0), 10);
    planes.at(1)->setState(wind::Vector3(0.0, -5.0, 0.0), wind::Vector3(0.0, 1.0, 0.0));
    player1->setState(wind::Vector3(0.0, 0.0, 0.0), wind::Vector3(0.5, 3.0, 0.5));
    player1->gravityOff();

    tester = "Find the blocks!";
}

void Game::handleEvents()
{
    wind::real x = 0.0;
    wind::real y = 0.0;
    while (SDL_PollEvent(&_input))
    {
        switch (_input.type)
        {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            if (!timerFlag)
            {
                countDown.start();
                timerFlag = true;
            }
            switch (_input.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                running = false;
                break;

            case SDL_SCANCODE_W:
                //player1[0]->move(wind::Vector3(0.0, 0.0, 20.0).moveForward(cameraRot));
                x = -20.0;
                break;

            case SDL_SCANCODE_S:
                //player1[0]->move(wind::Vector3(0.0, 0.0, -20.0).moveForward(cameraRot));
                x = 20.0;
                break;

            case SDL_SCANCODE_A:
                //player1[0]->move(wind::Vector3(20.0, 0.0, 0.0).moveSidewards(cameraRot));
                y = 20.0;
                break;

            case SDL_SCANCODE_D:
                //player1[0]->move(wind::Vector3(-20.0, 0.0, 0.0).moveSidewards(cameraRot));
                y = -20.0;
                break;

            case SDL_SCANCODE_R:
                resetGame();
                break;

            default:
                break;

            };
            player1->move(wind::Vector3(y, 0.0, x).moveSidewards(cameraRot));
            break;

        case SDL_KEYUP:
            switch (_input.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                break;

            case SDL_SCANCODE_W:
                //player1[0]->move(wind::Vector3(0.0, 0.0, 0.0));

            case SDL_SCANCODE_S:
                //player1[0]->move(wind::Vector3(0.0, 0.0, 0.0));
                x = 0.0;
                break;

            case SDL_SCANCODE_A:
                //player1[0]->move(wind::Vector3(0.0, 0.0, 0.0));

            case SDL_SCANCODE_D:
                //player1[0]->move(wind::Vector3(0.0, 0.0, 0.0));
                y = 0.0;
                break;

            case SDL_SCANCODE_R:
                break;

            default:
                break;
            };
            player1->move(wind::Vector3(x, 0.0, y));
            break;

        case SDL_JOYAXISMOTION:
            if (_input.jaxis.which == 0)
            {
                if (_input.jaxis.axis == 0)
                {
                    if (_input.jaxis.value < -JOYSTICK_DEAD_ZONE)
                    {
                    }
                    //Above of dead zone
                    else if (_input.jaxis.value > JOYSTICK_DEAD_ZONE)
                    {
                    }
                    else
                    {
                    }
                }

                if (_input.jaxis.axis == 1)
                {
                    if (_input.jaxis.value < -JOYSTICK_DEAD_ZONE)
                    {
                    }
                    //Above of dead zone
                    else if (_input.jaxis.value > JOYSTICK_DEAD_ZONE)
                    {
                    }
                    else
                    {
                    }
                }

            }
            break;
        };

        if (_input.type == SDL_MOUSEMOTION)
        {
            wind::real camYaw = 0.0;
            wind::real camPitch = 0.0;
            int tmpx, tmpy = 0;
            SDL_GetMouseState(&tmpx, &tmpy);
            camYaw += mousevel * (MidX - tmpx);
            camPitch += mousevel * (MidY - tmpy);

            bool rotY = camPitch != 0;
            bool rotX = camYaw != 0;

            if (rotY)
            {
                if (((cameraRot.getUp().z > -0.9 || -camPitch > 0.0)) && ((cameraRot.getUp().z < 0.9 || -camPitch < 0.0)))
                {
                    cameraRot *= Quaternion().initRotation(cameraRot.getRight(), -camPitch);
                }
            }
            if (rotX)
            {
                cameraRot *= Quaternion().initRotation(wind::Vector3::X, -camYaw);
            }

            cameraRot.normalise();
            player1->rotate(cameraRot);

            SDL_WarpMouseInWindow(_window, MidX, MidY);
        }
    }
}

void Game::update()
{
    RigidBodyApplication::update();

    if (trans.empty())
    {
        for (unsigned int i = 0; i < objects.size(); i++)
        {
            trans.push_back(std::move(std::unique_ptr<wind::RigidBody>(objects.at(i)->getBody())));
        }

        for (unsigned int i = 0; i < planes.size(); i++)
        {
            trans.push_back(std::move(std::unique_ptr<wind::RigidBody>(planes.at(i)->getBody())));
        }

        trans.push_back(std::move(std::unique_ptr<wind::RigidBody>(player1->getBody())));
    }

    gameLogic();
}

void Game::Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.bind();
    scene.disableBlend();
    scene.setTextColor(levelColour);
    scene.updateCamera(player1->getCamera());
    blockTexture.bind(0);
    scene.drawModels(objects);
    scene.drawModel(player1);
    blockTexture.unbind(0);
    texture.bind(0);
    scene.drawModels(planes);
    texture.unbind(0);
    scene.unbind();

    fontProgram2D.bind();
    fontProgram2D.enableBlend();
    fontProgram2D.setTextColor(textColour);

    font.renderText(&fontProgram2D, 0, 0, tester, &screenRect, textPosition);
    fontProgram2D.unbind();

    fontTimer2D.bind();
    fontTimer2D.setModelView(wind::Matrix4x4());
    fontTimer2D.updateModelView();
    fontTimer2D.setTextColor(textColour);
    font.renderText(&fontTimer2D, 0, 0, TimerString.str(), &screenRect, FONT_TEXT_ALIGN_LEFT);
    fontTimer2D.unbind();

    SDL_GL_SwapWindow(_window);
}

void Game::mainLoop()
{
    while (running)
    {
        handleEvents();
        update();
        Display();
    }
}

}; //wind