#ifndef _game_H
#define _game_H

#include <iostream>

struct GameObject;

typedef GameObject* GOPtr;

class Game
{
    std::string mContentFolderDir;
	GOPtr       mCurrentViewer;
public:
    Game(std::string path);
	~Game();

    void update(float deltaTime);
    void draw();
	void resize(int width, int height);
};

#endif
