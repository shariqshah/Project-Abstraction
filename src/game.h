#ifndef _game_H_
#define _game_H_

#include <vector>
#include <memory>

#include "systems.h"
#include "utilities.h"

typedef std::shared_ptr<GameObject> GOPtr;
typedef std::vector<GOPtr> ObjectList;

class Game
{
	ObjectList mObjectList;
    H3DNode mKnightNode;
    H3DRes mKnightModelRes;
	H3DRes mFontMatRes;
	H3DRes mPanelMatRes;
	H3DRes mLogoMatRes;
    std::string mContentFolderDir;
public:
    Game(std::string path);

    void update(float deltaTime);
    void draw();
	void resize(int width, int height);
};

#endif
