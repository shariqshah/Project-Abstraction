#ifndef RENDERER_H
#define RENDERER_H

#include "log.h"
#include "settings.h"
#include "mathdefs.h"
#include "datatypes.h"

#include <iostream>
#include <vector>
#include <algorithm>

typedef std::vector<Node> NodeList;

struct CCamera;

enum class DebugLevel : uint8_t
{
	NONE   = 0,
	MEDIUM = 1,
	HIGH   = 2
};

struct Rect
{
	std::string text     = "UNINITIALIZED";
	Vec2        scale    = Vec2(1, 1);
	Vec2        position = Vec2(0, 0);	
	Vec4        color    = Vec4(1.f);
	Mat4        transMat;
};

enum FogMode
{
	FG_NONE = 0,
	FG_LINEAR,
	FG_EXP,
	FG_EXP_SQRD
};

struct Fog
{
	int   fogMode = FG_EXP;
	float density = 0.01f;
	float start   = 50.f;
	float max     = 150.f;
	Vec4  color   = Vec4(60.f/255.f, 60.f/255.f, 75.f/255.f, 1.0f);
};

struct RenderParams
{
	Fog  fog;
	Vec4 ambientLight = Vec4(0.1f, 0.1f, 0.12f, 1.0f);
};

namespace Renderer
{	
    void initialize(const char* path);
	void cleanup();
    void addTextRect(Rect text);
	void checkGLError(const char* context);
	void renderFrame();
	void setClearColor(const Vec4 clearColor);
	Vec4 getClearColor();
	void addText(const std::string& text);
	void setDebugLevel(DebugLevel level);
	void toggleDebugView();
	void toggleWireframe();
	RenderParams* getRenderParams();
}

#endif
