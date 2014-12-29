#ifndef _game_H
#define _game_H

struct GameObject;

typedef GameObject* GOPtr;

class Game
{
	GOPtr        mCurrentViewer;
	unsigned int mVAO;
	unsigned int mShader;
	unsigned int mTexture;
public:
    Game(const char* path);
	~Game();

    void update(float deltaTime, bool* quit);
    void draw();
	void resize(int width, int height);
};

#endif
