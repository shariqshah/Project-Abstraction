#ifndef _game_H
#define _game_H

#include <iostream>

struct GameObject;

typedef uint32_t Node;

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
