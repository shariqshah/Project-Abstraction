#ifndef _game_H_
#define _game_H_

#include "systems.h"

class Game
{
    std::string mContentFolderDir;
	GOPtr mCurrentViewer;
public:
    Game(std::string path);
	~Game();

    void update(float deltaTime);
    void draw();
	void resize(int width, int height);
};

#endif
