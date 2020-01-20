#include "Application.h"

/******************************************************************************/
Application::Application(const std::string& title, int w, int h) : _test(gluNewQuadric()), 
    _width(w), _height(h),
    _ratio(static_cast<float>(_width) / static_cast<float>(_height)),
    _title(title)
{
    setup();
}

/******************************************************************************/
Application::~Application()
{
}

/******************************************************************************/
std::string Application::getTitle()
{
    return _title;
}

/******************************************************************************/
void Application::setup()
{
    //This sets up what openGL version we are going to use, in this case it's 3.3 that why it say that.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
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

        _window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                   _width, _height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if(_window == nullptr)
        {
            std::cerr << "Window could not be started for some reason" << std::endl;
        }
        else
        {
            //This is how we start up the render.
            _context = SDL_GL_CreateContext(_window);
            if(_context == nullptr)
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
                if(!GLEW_VERSION_3_2)
                {
                    printf( "OpenGL 3.2 not supported!\n" );
                }

                ilInit();
                iluInit();
                ilClearColour(255, 255, 255, 000);

                //Check for error
                ILenum ilError = ilGetError();
                if(ilError != IL_NO_ERROR)
                {
                    std::cerr << "Error initializing DevIL! " << iluErrorString(ilError) << std::endl;
                }

                initGraphics();

                controllerSetup();

                //Might be a better way than this in a different API
                //SDL_Surface *icon = Loadimage("res/icon.png");
                //SDL_SetWindowIcon(_window, icon);
            }
        }
    }
}

/******************************************************************************/
void Application::initGraphics()
{
    glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glViewport(0.f, 0.f, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height));

    /**
    glEnable(GL_TEXTURE_2D);
    //Set blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    */
    //SetView();
}

/******************************************************************************/
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
        _gameController = SDL_JoystickOpen(0);
        if(_gameController == NULL)
        {
            std::cerr << "Could not open controller " << SDL_GetError() << std::endl;
        }
        else
        {
            _controllerHaptic = SDL_HapticOpenFromJoystick(_gameController);
            if(_controllerHaptic == NULL)
            {
                std::cerr <<  "Warning: Controller does not support haptics! SDL Error: " 
                    << SDL_GetError() << std::endl;
            }
            else
            {
                //Get initialize rumble
                if(SDL_HapticRumbleInit(_controllerHaptic) < 0 )
                {
                    std::cerr <<  "Warning: Unable to initialize rumble! SDL Error: " 
                        << SDL_GetError() << std::endl;
                }
            }
        }
    }
}

/******************************************************************************/
//LegacyGL setup of perspective.
void Application::setView()
{
    //Currrently used for testing OpenGL. 
    //This does not set the view in the engine.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glPushMatrix();
    gluPerspective(60.0, _ratio, 0.2, 500.0);
    glViewport(0, 0, _width, _height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
}

/******************************************************************************/
void Application::display()
{
    //This is a test to see if OpenGL is working at all.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(0.1f, 0.9f, 0.7f);
    glBegin(GL_QUADS);
        glVertex3f(-5.f, -5.f, -15.f);
        glVertex3f(5.f, -5.f, -15.f);
        glVertex3f(5.f, 5.f, -15.f);
        glVertex3f(-5.f, 5.f, -15.f);
    glEnd();
}

/******************************************************************************/
void Application::update()
{
}

/******************************************************************************/
void Application::resize(int width, int height)
{
    if(height <= 0)
    {
        height = 1;
    }

    _width = width;
    _height = height;
    glViewport(0, 0, _width, _height);
    setView();
}

/******************************************************************************/
MassAggregateApplication::MassAggregateApplication(unsigned int ParticleCount, const std::string& Title, int w, int h) :
    Application(Title, w, h), _world(ParticleCount * 10)
{
    _particleArray = new wind::Particle[ParticleCount];
    for(unsigned int i = 0; i < ParticleCount; i++)
    {
        _world.getParticles().push_back(&_particleArray[i]);
    }

    _groundContactGenerator.Init(&_world.getParticles());
    _world.getContacts().push_back(&_groundContactGenerator);
}

/******************************************************************************/
MassAggregateApplication::~MassAggregateApplication()
{
    delete [] _particleArray;
}

/******************************************************************************/
void MassAggregateApplication::initGraphics()
{
    //Here we just make sure we inherent the applications init graphics function which we still need.
    Application::initGraphics();
}

/******************************************************************************/
void MassAggregateApplication::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 3.5, 9.0, 0.0, 3.5, 0.0, 0.0, 1.0, 0.0);
    glColor3f(0.0f, 0.0f, 0.0f);

    wind::ParticleWorld::Particles &particles = _world.getParticles();
    for(wind::ParticleWorld::Particles::iterator p = particles.begin(); p != particles.end(); p++)
    {
        wind::Particle* particle = *p;
        const wind::Vector3 &pos = particle->GetPosition();
        glPushMatrix();
        glTranslated(pos.x, pos.y, pos.z);
        gluSphere(_test, 0.040, 20, 10);
        glPopMatrix();
    }
}

/******************************************************************************/
void MassAggregateApplication::update(wind::real Duration)
{
    _world.startFrame();

    _world.runPhysics(Duration);
}

/******************************************************************************/
RigidBodyApplication::RigidBodyApplication(const std::string& title, int w, int h) : 
    Application(title, w, h), _resolver(8 * MAX_CONTACTS), _theta(0.f),
    _alpha(15.f), _xLastPos(0), _yLastPos(0), _pausePhysics(false),
    _autoPausePhysics(false), _renderDebugInfo(false)
{
    _collData.contactArray = _contacts;

    _physicsClock.Start();
}

/******************************************************************************/
//Legacy OpenGL for debugging.
void RigidBodyApplication::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(18.0f, 0, 0,  0, 0, 0,  0, 1.0f, 0);
    glRotatef(-_alpha, 0, 0, 1);
    glRotatef(_theta, 0, 1, 0);
    glTranslatef(0, -5.0f, 0);
}

/******************************************************************************/
void RigidBodyApplication::update()
{
    wind::real Duration = _physicsClock.GetTicks() * 0.001f;
    if (Duration > 0.0)
    {
        //Here we simply stop the physics simulation if we are paused.
        if (_pausePhysics == false)
        {
            //Here is where all the meat happens we generate contacts and update them.
            _physicsClock.Update();

            updateObjects(Duration);

            generateContacts();

            //Here we resolve all contacts.
            _resolver.resolveContact(_collData.contactArray, _collData.contactCount, Duration);
        }
        else if (_autoPausePhysics)
        {
            _pausePhysics = true;
            _autoPausePhysics = false;
        }
        else 
        {
            //Do no physics option.
            Application::update();
        }
    }
}
