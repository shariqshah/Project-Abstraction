#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#include "game.h"
#include "settings.h"
#include "input.h"
#include "renderer.h"
#include "log.h"
#include "editor.h"

//========================================================> Globals
SDL_Window*   window = NULL;
SDL_GLContext context;
Game*         game;

//========================================================> Function Prototypes
bool        init();
void        close();
void        handleEvents(SDL_Event* event, bool *quit);
void        handleWindowEvent(SDL_WindowEvent event);
char*       getWorkingDirectory();

int main(int argc, char** args)
{
	bool quit = false;

    //Initialize SDL and OpenGL
    if(!init())
    {
		Log::error("Main::main", "Could not initialize");
    }
    else
    {
        //Event handler
        SDL_Event event;
        float current, previous, deltaTime;
		current = previous = deltaTime = 0.f;
		
        while(!quit)
        {
            previous = current;
            current = SDL_GetTicks();
            deltaTime = (current - previous) / 1000.f;

            //Handle events on a queue
            handleEvents(&event, &quit);

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

			Editor::setUpdateTime(updateTime);
			Editor::setDrawTime(drawTime);
			
            //Swap Buffers and update screen
            SDL_GL_SwapWindow(window);
        }
    }
    //Free resources
    close();
	Log::message("Program exiting!");
    return 0;
}

bool init()
{
	bool success = true;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	//Initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
		Log::error("Main::init", "SDL could not initialize. " + std::string(SDL_GetError()));
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
									  SDL_WINDOW_SHOWN  |          //Window flags, show window when created
									  SDL_WINDOW_OPENGL |          //OpenGL flag
									  SDL_WINDOW_RESIZABLE );
			
			if(window == NULL)
			{
				Log::error("Main::init", "Window could not be created. " + std::string(SDL_GetError()));
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
					Log::error("Main::init", "Creating OpenGL context failed " + std::string(SDL_GetError()));
					success = false;
				}
				else
				{
					//Initialize GLEW
					glewExperimental = GL_TRUE;
					GLenum glewError = glewInit();
					if( glewError != GLEW_OK )
							printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));

					char* directory = getWorkingDirectory();
					game = new Game(directory);
					free(directory);
					Input::initializeInput();
				}
			}	
		}
		else
		{
			success = false;
			Log::error("Main::init", "Error loading settings");
		}
	}
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
		if(event->type == SDL_QUIT)
			*quit = true;

		if(event->type == SDL_KEYDOWN || event->type == SDL_KEYUP)
			Input::updateKeys(event->key);

		if(event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP)
			Input::updateMouseButtons(event->button);

		if(event->type == SDL_MOUSEMOTION)
			Input::updateMousePostion(event->motion);

		if(event->type == SDL_MOUSEWHEEL)
			Input::updateScroll(event->wheel);

		if(event->type == SDL_TEXTINPUT)
			Input::textEntered(&event->text.text[0]);

		if(event->type == SDL_WINDOWEVENT)
			handleWindowEvent(event->window);
	}
}

char* getWorkingDirectory()
{
	long size;
	char *buf;
	char *ptr;
	size = pathconf(".", _PC_PATH_MAX);

	if ((buf = (char *)malloc((size_t)size)) != NULL)
		ptr = getcwd(buf, (size_t)size);

	return buf;
}

void handleWindowEvent(SDL_WindowEvent winEvent)
{
	switch(winEvent.event)
	{
	    case SDL_WINDOWEVENT_SIZE_CHANGED:
			game->resize(winEvent.data1, winEvent.data2);
			Settings::setWindowWidth(winEvent.data1);
			Settings::setWindowHeight(winEvent.data2);
			break;
		
	};
}
