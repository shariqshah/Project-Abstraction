#include <GL/glew.h>
#include <GL/gl.h>

#include "game.h"
#include "systems.h"
#include "renderer.h"
#include "camera.h"
#include "model.h"
#include "physics.h"
#include "light.h"
#include "transform.h"
#include "scenemanager.h"
#include "gameobject.h"
#include "camerasystems.h"
#include "cpu.h"
#include "utilities.h"
#include "shader.h"
#include "texture.h"
#include "gui.h"
#include "scriptengine.h"
#include "passert.h"
#include "collisionshapes.h"
#include "framebuffer.h"

Game::Game(const char* path)
{
	Renderer::initialize(path);
	System::initialize();
	Gui::initialize();
	
	SceneManager::loadScene("../content/scenes/savetest.json");
	int fbo = Framebuffer::create(1024, 768);
}

Game::~Game()
{
	System::cleanup();
	Renderer::cleanup();
	Gui::cleanup();
}

void Game::update(float deltaTime, bool* quit)
{
	Gui::update(deltaTime);
	System::update(deltaTime, quit);
}

void Game::draw()
{
	Renderer::renderFrame();
	Physics::draw();
	Gui::render();
}

void Game::resize(int width, int height)
{
	glViewport(0, 0, width, height); // TODO: Probably move viewport stuff to camera?
	Camera::updateAllCamerasAspectRatio((float)(width/height));
	Gui::resize();
}
