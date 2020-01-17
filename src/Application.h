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
#include "SDL.h"
#include "SDL_opengl.h"
#include <IL/il.h>
#include <IL/ilu.h>

#include "Physics/include/Wind.h"
#include "Physics/CollisionSystem/contact.h"
#include "Physics/CollisionSystem/collision_broad.h"
#include "Physics/CollisionSystem/collision_narrow.h"

#include "Timer.h"

class Application
{
    public:
        Application(const std::string& Title);
        ~Application();

        //Simple returns an array of chars that handle the title.
        virtual std::string GetTitle();

        virtual void Setup(const std::string& Title = "");

        //This function draws stuff to the screen every frame.
        //Also by default this function draw a straight line across the screen so you can see OpenGL is working.
        virtual void Display();

        //This function flushes the screen every frame and updates it.
        virtual void Update();

        //This function resizes the screen and updates the gl viewpoint.
        virtual void Resize(int width, int height);

	protected:
		//Sets up all the graphics ready to be rendered to the screen.
        //Needs to be called as soon as OpenGL has been started.
        virtual void InitGraphics();

        //This function sets up the projection characteristics.
        //By default the camera has 60 degree view with 2 to 500 units of range.
        virtual void SetView();

		//This function sets up the controller with SDL at the helm. Make sure you build this at the end of the setup.
		void controllerSetup();

		//This function just handles loading the image for the icon
		//SDL_Surface *Loadimage(std::string filename);

		SDL_GLContext Context;
        SDL_Window* mWindow;
        SDL_Event Input;
		GLUquadric* Test;

		//These two variables start the set up for the controller.
		SDL_Joystick* gameController;
		SDL_Haptic* controllerHaptic;

		float mRatio;

        //Height and width of the screen.
        int mHeight;
        int mWidth;

};
/**
    Just like the application this is the super class for any application that will use The Mass Aggregate Engine.
    Also this class will support a basic testing ground to see if the application works.
*/
class MassAggregateApplication : public Application
{
    protected:
        wind::ParticleWorld mWorld;
        wind::Particle* mParticleArray;
        wind::GroundContacts mGroundContactGenerator;

    public:
        MassAggregateApplication(unsigned int ParticleCount, const std::string& Title);
        virtual ~MassAggregateApplication();

        //Sets up all the graphics ready to be rendered to the screen.
        //Needs to be called as soon as OpenGL has been started.
        virtual void InitGraphics();

        //This function draws stuff to the screen every frame.
        //Also by default this function draw a straight line across the screen so you can see OpenGL is working.
        virtual void Display();

        //This function flushes the screen every frame and updates it.
        virtual void Update(wind::real Duration);

};


/**
	Like all the other application layer classes this class handles general case rigid body information.
	This class supports, camera, creation of new contacts, limits the number of bodies to 256.
	Also some virtal base functions to force the physics to work in the proper way.
*/
class RigidBodyApplication : public Application
{
	protected:
		//Total number of bodies that can happen.
		const static unsigned int maxContacts = 256;

		//Here we have the all the contacts we will be working with.
		wind::Contact contacts[maxContacts];

		//This holds the structure for our collision detector.
		wind::CollisionData collData;

		//Here we have the structure that holds the collision resolver.
		wind::ContactResolver resolver;

		//Here we have to 2 values for moving the camera, theta is the angle and alpha is the elevation.
		float theta;

		float alpha;

		//Holds the last position of the mouse.
		int xLastPos, yLastPos;

		//Next we need to know what state the physics engine needs to be in.
		bool pausePhysics;

		bool autoPausePhysics;

		//This one is important for debugging, do we need to see the how the contacts are work? If yes we need this to be true.
		bool renderDebugInfo;

		/**Virtual base functions that need to be include in every physics demo*/
		//This function processes the contact generation code.
		virtual void generateContacts() = 0;

		//This function processes the objects in the simulation forward in time.
		virtual void updateObjects(wind::real duration) = 0;

		//This function resets all the physics back to where is was.
		virtual void reset() = 0;

		//Holds the time for the physics
		Timer mPhysicsClock;
	public:
		//All functions here are simple setup functions we can be changed in any project.
		//Creates a new application object.
		RigidBodyApplication(const std::string& title);

		//Display the application.
		virtual void display();

		//Update the objects.
		virtual void update();
};

#endif
