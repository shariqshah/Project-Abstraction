#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include "game.h"
#include "settings.h"
#include "input.h"
#include "renderer.h"

//========================================================> Globals
SDL_Window*   window = NULL;
SDL_GLContext context;
Game*         game;

//========================================================> Function Prototypes
bool        init(char *pFullPath);
bool        initGL();
void        close();
void        handleEvents(SDL_Event* event, bool *quit);
void        handleWindowEvent(SDL_WindowEvent event);
std::string extractAppPath(char *pFullPath);

int main(int argc, char** args)
{
	bool quit = false;

    //Initialize SDL and OpenGL
    if(!init(args[0]))
    {
		std::cerr<<"Could not initialize \n";
    }
    else
    {
        //Event handler
        SDL_Event e;
        float current, previous, deltaTime;
		current = previous = deltaTime = 0.f;
		
        while(!quit)
        {
            previous = current;
            current = SDL_GetTicks();
            deltaTime = (current - previous) / 1000.f;

            //Handle events on a queue
            handleEvents(&e, &quit);

            //Render to screen
			float timeBeforeUpdate = SDL_GetTicks();
			game->update(deltaTime, &quit);
			float timeAfterUpdate = SDL_GetTicks();
			float updateTime = timeAfterUpdate - timeBeforeUpdate;
			
			float timeBeforeDraw = SDL_GetTicks();
			game->draw();
			float timeAfterDraw = SDL_GetTicks();
			float drawTime = timeAfterDraw - timeBeforeDraw;
			
			//Set released keys back to inactive
			Input::updateReleasedKeys();
			Input::resetMouseRel();

			Renderer::addText("Update Time : " + std::to_string(updateTime));
			Renderer::addText("Draw Time : " + std::to_string(drawTime));
			
            //Swap Buffers and update screen
            SDL_GL_SwapWindow(window);
        }
    }

    //Free resources
    close();
	std::cout<<"Program exiting!"<<std::endl;

    return 0;
}

bool init(char *pFullPath)
{
	bool success = true;
	
	//Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
		std::cerr<<"ERROR : SDL could not initialize. "<<SDL_GetError()<<std::endl;
        success = false;
    }
    else
    {
		if(Settings::initSettings())
		{
			//Create Window
			window = SDL_CreateWindow("ProjectAbstraction",        //Window title
									  SDL_WINDOWPOS_CENTERED,      //Window position x
									  SDL_WINDOWPOS_CENTERED,      //Window position y
									  Settings::getWindowWidth(),  //Window width
									  Settings::getWindowHeight(), //Window height
									  SDL_WINDOW_SHOWN |           //Window flags, show window when created
									  SDL_WINDOW_OPENGL);          //OpenGL flag
			
			if(window == NULL)
			{
				std::cerr<<"ERROR : Window could not be created. "<<SDL_GetError()<<std::endl;
				success = false;
			}
			else
			{
				//Create OpenGl context
				context = SDL_GL_CreateContext(window);

				// Enable vsync
				SDL_GL_SetSwapInterval(1);
				
				if(context == NULL)
				{
					std::cerr<<"ERROR : Creating OpenGL context, "<<SDL_GetError()<<std::endl;
					success = false;
				}
				else
				{
					//Initialize GLEW
					glewExperimental = GL_TRUE;
					GLenum glewError = glewInit();
					if( glewError != GLEW_OK )
							printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));

					// if(GLEW_ARB_uniform_buffer_object)
					// 	Log::message("GLEW_ARB_uniform_buffer_object = YES\n");
					// else
					// 	Log::message("GLEW_ARB_uniform_buffer_object = NO\n");

					//Initialize OpenGL
					if(!initGL())
					{
						Log::error("Init GL", "Unalble to initialize OpenGL");
						success = false;
					}
					else
					{
						game = new Game(extractAppPath(pFullPath));
						Input::initializeInput();
					}
				}
			
			}	
		}
	}

	return success;
}

bool initGL()
{
    bool success = true;

    glClearColor(0.55, 0.6, 0.8, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);

    return success;
}

void close()
{
	//game->cleanup();
	delete game;
	
	//Destroy window
    SDL_DestroyWindow(window);
    window = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void handleEvents(SDL_Event* event, bool *quit)
{
    while(SDL_PollEvent(event) != 0)
    {
		//User request quit
		if(event->type == SDL_QUIT)
			*quit = true;

		if(event->type == SDL_KEYDOWN)
			Input::updateKeys(event->key);

		if(event->type == SDL_KEYUP)
			Input::updateKeys(event->key);

		if(event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP)
			Input::updateMouseButtons(event->button);

		if(event->type == SDL_MOUSEMOTION)
			Input::updateMousePostion(event->motion);

		if(event->type == SDL_WINDOWEVENT)
			handleWindowEvent(event->window);
	}
}

std::string extractAppPath( char *pFullPath )
{
#ifdef __APPLE__
	std::string s( pFullPath );
	for( int i = 0; i < 4; ++i )
		s = s.substr( 0, s.rfind( "/" ) );
	return s + "/../";
#else
	const std::string s( pFullPath );
	if( s.find( "/" ) != std::string::npos )
		return s.substr( 0, s.rfind( "/" ) ) + "/";
	else if( s.find( "\\" ) != std::string::npos )
		return s.substr( 0, s.rfind( "\\" ) ) + "\\";
	else
		return "";
#endif
}

void handleWindowEvent(SDL_WindowEvent winEvent)
{
	switch(winEvent.event)
	{
	    case SDL_WINDOWEVENT_SIZE_CHANGED:
			game->resize(winEvent.data1, winEvent.data2);
			break;
		
	};
}
