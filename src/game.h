#ifndef _game_H
#define _game_H

#include <iostream>
#include "datatypes.h"

struct GameObject;

class Game
{
	Node player;
public:
    Game(const char* path);
	~Game();

    void update(float deltaTime, bool* quit);
    void draw();
	void resize(int width, int height);
};

#endif
