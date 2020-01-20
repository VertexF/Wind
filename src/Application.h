#ifndef APPLICATION_H
#define APPLICATION_H

#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include "Physics/include/Wind.h"
#include "Physics/CollisionSystem/contact.h"
#include "Physics/CollisionSystem/collision_broad.h"
#include "Physics/CollisionSystem/collision_narrow.h"

#include "Timer.h"

namespace
{
    //Total number of bodies that can happen.
    const unsigned int MAX_CONTACTS = 256;

}; //Anon

namespace wind
{
class Application
{
public:
    Application(const std::string& title, int w, int h);
    ~Application();

    //Simple returns an array of chars that handle the title.
    virtual std::string getTitle();

    virtual void setup();

    //This function draws stuff to the screen every frame.
    //Also by default this function draw a straight line across the screen so you can see OpenGL is working.
    virtual void display();

    //This function flushes the screen every frame and updates it.
    virtual void update();

    //This function resizes the screen and updates the gl viewpoint.
    virtual void resize(int width, int height);

protected:
    //Sets up all the graphics ready to be rendered to the screen.
    //Needs to be called as soon as OpenGL has been started.
    virtual void initGraphics();

    //This function sets up the projection characteristics.
    //By default the camera has 60 degree view with 2 to 500 units of range.
    virtual void setView();

    //This function sets up the controller with SDL at the helm. Make sure you build this at the end of the setup.
    void controllerSetup();

    //This function just handles loading the image for the icon
    //SDL_Surface *Loadimage(std::string filename);

    SDL_GLContext _context;
    SDL_Window* _window;
    SDL_Event _input;
    GLUquadric* _test;

    //These two variables start the set up for the controller.
    SDL_Joystick* _gameController;
    SDL_Haptic* _controllerHaptic;

    //title of the window.
    std::string _title;
    float _ratio;

    //Height and width of the screen.
    int _height;
    int _width;
};

/**
    Just like the application this is the super class for any application that will use The Mass Aggregate Engine.
    Also this class will support a basic testing ground to see if the application works.
*/
class MassAggregateApplication : public Application
{
public:
    MassAggregateApplication(unsigned int ParticleCount, const std::string& Title, int w, int h);
    virtual ~MassAggregateApplication();

    //Sets up all the graphics ready to be rendered to the screen.
    //Needs to be called as soon as OpenGL has been started.
    virtual void initGraphics();

    //This function draws stuff to the screen every frame.
    //Also by default this function draw a straight line across the screen so you can see OpenGL is working.
    virtual void display();

    //This function flushes the screen every frame and updates it.
    virtual void update(wind::real Duration);

protected:
    wind::ParticleWorld _world;
    wind::Particle* _particleArray;
    wind::GroundContacts _groundContactGenerator;

};

/**
    Like all the other application layer classes this class handles general case rigid body information.
    This class supports, camera, creation of new contacts, limits the number of bodies to 256.
    Also some virtal base functions to force the physics to work in the proper way.
*/
class RigidBodyApplication : public Application
{
public:
    //All functions here are simple setup functions we can be changed in any project.
    //Creates a new application object.
    RigidBodyApplication(const std::string& title, int w, int h);

    //Display the application.
    virtual void display();

    //Update the objects.
    virtual void update();

protected:
    //Here we have the all the contacts we will be working with.
    wind::Contact _contacts[MAX_CONTACTS];

    //This holds the structure for our collision detector.
    wind::CollisionData _collData;

    //Here we have the structure that holds the collision resolver.
    wind::ContactResolver _resolver;

    //Here we have to 2 values for moving the camera, theta is the angle and alpha is the elevation.
    float _theta;
    float _alpha;

    //Holds the last position of the mouse.
    int _xLastPos, _yLastPos;

    //Next we need to know what state the physics engine needs to be in.
    bool _pausePhysics;
    bool _autoPausePhysics;

    //This one is important for debugging, do we need to see the how the contacts are work? If yes we need this to be true.
    bool _renderDebugInfo;

    /**Virtual base functions that need to be include in every physics demo*/
    //This function processes the contact generation code.
    virtual void generateContacts() = 0;

    //This function processes the objects in the simulation forward in time.
    virtual void updateObjects(wind::real duration) = 0;

    //This function resets all the physics back to where is was.
    virtual void reset() = 0;

    //Holds the time for the physics
    Timer _physicsClock;
};
};
#endif
