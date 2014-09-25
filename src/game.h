#ifndef _game_H_
#define _game_H_

#include <memory>

#include "renderer.h"

#include "camera.h"
#include "utilities.h"
#include "log.h"

class Game
{
    H3DNode mKnightNode;
    H3DRes mForwardPipeRes, mKnightModelRes;
	H3DRes mFontMatRes;
	H3DRes mPanelMatRes;
	H3DRes mLogoMatRes;
    std::string mContentFolderDir;

	std::shared_ptr<Camera> mCamera;
public:
    Game(std::string path);

    void update(float deltaTime, SDL_Event *event);
    void draw();
	void resize(int width, int height);
};

#endif
