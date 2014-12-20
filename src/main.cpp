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
void        close();
void        handleKeyboard(SDL_Event event, float deltaTime, bool *quit);
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

        while(!quit)
        {
            previous = current;
            current = SDL_GetTicks();
            deltaTime = (current - previous) / 1000.f;

            //Handle events on a queue
            while(SDL_PollEvent(&e) != 0)
            {
                //User request quit
                if(e.type == SDL_QUIT)
                    quit = true;

                if(e.type == SDL_KEYDOWN)
                {
                    handleKeyboard(e, deltaTime, &quit);
					Input::updateKeys(e.key);
                }

				if(e.type == SDL_KEYUP)
					Input::updateKeys(e.key);

				if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
					Input::updateMouseButtons(e.button);

				if(e.type == SDL_MOUSEMOTION)
					Input::updateMousePostion(e.motion);

				if(e.type == SDL_WINDOWEVENT)
					handleWindowEvent(e.window);
            }
			

            //Render to screen
			float timeBeforeUpdate = SDL_GetTicks();
			game->update(deltaTime);
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
					if(!h3dInit())
					{
						h3dutDumpMessages();
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

void close()
{
	//game->cleanup();
	delete game;
	
	// Release horde3d
	h3dRelease();
	
	//Destroy window
    SDL_DestroyWindow(window);
    window = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void handleKeyboard(SDL_Event event, float deltaTime, bool *quit)
{
    switch(event.key.keysym.sym)
    {
    case SDLK_ESCAPE:
        *quit = true;
        break;
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
