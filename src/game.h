#ifndef _game_H_
#define _game_H_

#include "systems.h"
#include "model.h"
#include "scenemanager.h"

class Game
{
    std::string mContentFolderDir;
public:
    Game(std::string path);
	~Game();

    void update(float deltaTime);
    void draw();
	void resize(int width, int height);
};

#endif
