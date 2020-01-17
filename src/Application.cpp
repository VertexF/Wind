#include "Application.h"

Application::Application(const std::string& Title)
{
    Test = gluNewQuadric();
    mWidth = 800;
    mHeight = 600;

    mRatio = float(mWidth) / float(mHeight);

	Setup(Title);
}

Application::~Application()
{

}

std::string Application::GetTitle()
{
    return "OpenGL + GLU Application";
}

void Application::Setup(const std::string& Title)
{
    //This sets up what openGL version we are going to use, in this case it's 3.3 that why it say that.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "SDL failed to start" << std::endl;
    }
    else
    {
        //What these function do is set up the Attributes we are going to be working with.
        //There are default values to all these SDL_GL_ function.
        //This function works by taking in a attribute and then setting up the totally number of bits it will be using
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		mWindow = SDL_CreateWindow(Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWidth, mHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if(mWindow == NULL)
        {
            std::cerr << "Window could not be started for some reason" << std::endl;
        }
        else
        {
            //This is how we start up the render.
            Context = SDL_GL_CreateContext(mWindow);
            if(Context == NULL)
            {
                std::cerr << "Context could not be started for some reason" << std::endl;
            }
            else
            {
                //This is the vsync to stop the ripped animations.
                if(SDL_GL_SetSwapInterval(1) < 0)
                {
                    std::cerr << "Warning: Unable to set VSync!" << std::endl;
                }

				//Here we are starting up glew.
				glewExperimental = GL_TRUE;
				GLenum error = glewInit();
				if(GLEW_OK != error)
				{
					std::cerr << stderr << "You are have glew error :(" << std::endl;
				}

                //Make sure OpenGL 3.3 is supported
                if(!GLEW_VERSION_3_3)
                {
                    printf( "OpenGL 3.3 not supported!\n" );
                }

				//Initialize SDL_ttf
				//if(TTF_Init() == -1)
				//{
					//std::cerr << "TTF couldn't start" << std::endl;
				//}

				ilInit();
				iluInit();
				ilClearColour(255, 255, 255, 000);

				//Check for error
				ILenum ilError = ilGetError();
				if(ilError != IL_NO_ERROR)
				{
					std::cerr << "Error initializing DevIL! " << iluErrorString(ilError) << std::endl;
				}

				//glEnable(GL_TEXTURE_2D);
                InitGraphics();

				controllerSetup();

				//SDL_Surface *icon = Loadimage("res/icon.png");
    //            SDL_SetWindowIcon(mWindow, icon);
            }


        }
    }

}

void Application::InitGraphics()
{

	glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glEnable(GL_DEPTH_TEST);
	//glShadeModel(GL_SMOOTH);
	glViewport(0.f, 0.f, mWidth, mHeight);

	/**
	glEnable(GL_TEXTURE_2D);
	//Set blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	*/
    //SetView();
}

void Application::controllerSetup()
{
	//Set texture filtering to linear
	//if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ))
	//{
		//std::cerr << "Failed" << std::endl;
	//}
	if(SDL_NumJoysticks() < 1)
    {
        std::cerr << "No controller connected " << SDL_GetError() << std::endl;
    }
    else
    {
		std::cerr << "A controller is connected: Num of joy sticks is ";
		for(int i = 0; i < SDL_NumJoysticks(); i++)
		{
			std::cerr << i << std::endl;
		}

		SDL_JoystickEventState(SDL_ENABLE);
        gameController = SDL_JoystickOpen(0);
        if(gameController == NULL)
        {
            std::cerr << "Could not open controller " << SDL_GetError() << std::endl;
        }
		else
		{
			controllerHaptic = SDL_HapticOpenFromJoystick(gameController);
			if(controllerHaptic == NULL)
			{
				std::cerr <<  "Warning: Controller does not support haptics! SDL Error: " << SDL_GetError() << std::endl;
			}
			else
			{
				//Get initialize rumble
				if(SDL_HapticRumbleInit(controllerHaptic) < 0 )
				{
					std::cerr <<  "Warning: Unable to initialize rumble! SDL Error: " << SDL_GetError() << std::endl;
				}
			}
		}
    }
}

//SDL_Surface *Application::Loadimage(std::string filename)
//{
//    //SDL_Surface *Temp = NULL;
//
//    //Temp = IMG_Load(filename.c_str());
//    //if(Temp != NULL)
//    //{
//    //    return Temp;
//    //}
//    //else
//    //{
//    //    std::cerr << "Image could not correctly loaded" << std::endl;
//    //    return nullptr;
//    //}
//}

//LegacyGL setup of perspective.
void Application::SetView()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glPushMatrix();
    gluPerspective(60.0, mRatio, 0.2, 500.0);
    glViewport(0, 0, mWidth, mHeight);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
}

void Application::Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(0.1f, 0.9f, 0.7f);
    glBegin(GL_QUADS);
        glVertex3f(-5.f, -5.f, -15.f);
        glVertex3f(5.f, -5.f, -15.f);
        glVertex3f(5.f, 5.f, -15.f);
        glVertex3f(-5.f, 5.f, -15.f);
    glEnd();
}

void Application::Update()
{

}

void Application::Resize(int width, int height)
{
    if(height <= 0)
    {
        height = 1;
    }

    mWidth = width;
    mHeight = height;
    glViewport(0, 0, mWidth, mHeight);
    SetView();
}

MassAggregateApplication::MassAggregateApplication(unsigned int ParticleCount, const std::string& Title) : Application(Title), mWorld(ParticleCount * 10)
{
    mParticleArray = new wind::Particle[ParticleCount];
    for(unsigned int i = 0; i < ParticleCount; i++)
    {
        mWorld.getParticles().push_back(&mParticleArray[i]);
    }

    mGroundContactGenerator.Init(&mWorld.getParticles());
    mWorld.getContacts().push_back(&mGroundContactGenerator);
}

MassAggregateApplication::~MassAggregateApplication()
{
    delete [] mParticleArray;
}

void MassAggregateApplication::InitGraphics()
{
    //Here we just make sure we inherent the applications init graphics function which we still need.
    Application::InitGraphics();
}


void MassAggregateApplication::Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 3.5, 9.0, 0.0, 3.5, 0.0, 0.0, 1.0, 0.0);
    glColor3f(0.0f, 0.0f, 0.0f);

    wind::ParticleWorld::Particles &particles = mWorld.getParticles();
    for(wind::ParticleWorld::Particles::iterator p = particles.begin(); p != particles.end(); p++)
    {
        wind::Particle* particle = *p;
        const wind::Vector3 &pos = particle->GetPosition();
        glPushMatrix();
        glTranslated(pos.x, pos.y, pos.z);
        gluSphere(Test, 0.040, 20, 10);
        glPopMatrix();
    }
}

void MassAggregateApplication::Update(wind::real Duration)
{
    mWorld.startFrame();

    mWorld.runPhysics(Duration);
}

RigidBodyApplication::RigidBodyApplication(const std::string& title) : Application(title),
	resolver(8 * maxContacts),
	theta(0.f),
	alpha(15.f),
	xLastPos(0),
	yLastPos(0),

	pausePhysics(false),
	autoPausePhysics(false),
	renderDebugInfo(false)
{
	collData.contactArray = contacts;

	mPhysicsClock.Start();
}

void RigidBodyApplication::display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(18.0f, 0, 0,  0, 0, 0,  0, 1.0f, 0);
    glRotatef(-alpha, 0, 0, 1);
    glRotatef(theta, 0, 1, 0);
    glTranslatef(0, -5.0f, 0);
}

void RigidBodyApplication::update()
{
	wind::real Duration = mPhysicsClock.GetTicks() * 0.001f;
    if(Duration <= 0.0)
    {
        return;
    }

	//Here we simply stop the physics simulation if we are paused.
	if(pausePhysics)
	{
		Application::Update();
		return;
	}
	else if(autoPausePhysics)
	{
		pausePhysics = true;
		autoPausePhysics = false;
	}

	//Here is where all the meat happens we generate contacts and update them.
	mPhysicsClock.Update();

	updateObjects(Duration);

	generateContacts();

	//Here we resolve all contacts.
	resolver.resolveContact(collData.contactArray, collData.contactCount, Duration);
}
