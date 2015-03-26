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

Game::Game(const char* path)
{
	Renderer::initialize(path);
	System::initialize();
	Gui::initialize();
	
	{
		Vec4 color = Vec4(0, 1, 1, 1);
		GameObject* sphere = SceneManager::create("sphere");
		CModel* sphereModel = GO::addModel(sphere, "sphere.pamesh");
		sphereModel->materialUniforms.diffuseColor = color;
		CLight* light = GO::addLight(sphere);
		light->type  = LT_POINT;
		light->color = color;
		CTransform* modelTransform = GO::getTransform(sphere);
		Transform::setPosition(modelTransform, Vec3(-5, 10, -5), true);
	}

	{
		Vec4 color = Vec4(1, 0.6, 0, 1);
		GameObject* sphere = SceneManager::create("sphere2");
		CModel* sphereModel = GO::addModel(sphere, "sphere.pamesh");
		sphereModel->materialUniforms.diffuseColor = color;
		CLight* light = GO::addLight(sphere);
		light->type  = LT_POINT;
		light->color = color;
		CTransform* modelTransform = GO::getTransform(sphere);
		Transform::setPosition(modelTransform, Vec3(15 , 10, -5), true);
	}
	// CModel cubeModel;
	// cubeModel.material = MAT_PHONG;
	// cubeModel.materialUniforms.diffuseColor = Vec4(0, 1, 0, 1);
	// cubeModel.materialUniforms.texture = Texture::create("layingrock.png");
	// Renderer::Model::loadFromFile("suzanne.pamesh", &cubeModel);
	{
		GameObject* cube = SceneManager::create("Cube");
		CModel* cubeModel = GO::addModel(cube, "suzanne.pamesh");
		Renderer::Model::setMaterialType(cubeModel, MAT_PHONG);
		cubeModel->materialUniforms.diffuseColor = Vec4(0, 1, 0, 1);
		cubeModel->materialUniforms.texture = Texture::create("layingrock.png");
		CTransform* cubeTran = GO::getTransform(cube);
		Transform::setPosition(cubeTran, Vec3(5, 0, -5), true);
		ScriptEngine::addScript(cube, "test");
	}
	
	for(int i = 0; i < 10; i++)
	{
		GameObject* cube2 = SceneManager::create("Cube" + std::to_string(i));
		CModel* cubeModel = GO::addModel(cube2, "suzanne.pamesh");
		Renderer::Model::setMaterialType(cubeModel, MAT_PHONG);
		cubeModel->materialUniforms.diffuseColor = Vec4(0, 1, 0, 1);
		cubeModel->materialUniforms.texture = Texture::create("layingrock.png");
		if((i % 2) == 0)
		{
			cubeModel->materialUniforms.diffuseColor = Vec4(1, 0, 0, 1);
			cubeModel->material = MAT_PHONG;
		}
		else
		{
			cubeModel->materialUniforms.diffuseColor = Vec4(1);
		}
		CTransform* cube2Tran = GO::getTransform(cube2);
		Transform::setPosition(cube2Tran, Vec3(i, i + 2, -i), true);
	}
	
	GameObject* playerPtr = SceneManager::create("Player");
	player = playerPtr->node;
	playerPtr->tag = "FreeCamera";
	CTransform* transform = GO::getTransform(playerPtr);
	Transform::translate(transform, Vec3(0, 3, 5));
	CCamera* camera = GO::addCamera(playerPtr);
	Renderer::Camera::setActiveCamera(camera);
	System::CameraSystem::setActiveObject(playerPtr);

	GameObject* plane = SceneManager::create("Ground");
	CModel* planeModel = GO::addModel(plane, "plane.pamesh");
    Renderer::Model::setMaterialType(planeModel, MAT_PHONG);
	planeModel->materialUniforms.texture = Texture::create("test2.png");

	GameObject* tea = SceneManager::create("Teapot");
	CModel* teapot = GO::addModel(tea, "teapot.pamesh");
	Renderer::Model::setMaterialType(teapot, MAT_PHONG);
	teapot->materialUniforms.specularStrength = 60.f;
	teapot->materialUniforms.texture = Texture::create("chessboard.png");
	CTransform* tTran = GO::getTransform(tea);
	Transform::setPosition(tTran, Vec3(0, 10, -5), true);
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
	Gui::render();
	// Physics::draw(activeTrans, activeCamera);
}

void Game::resize(int width, int height)
{
	glViewport(0, 0, width, height); // TODO: Probably move viewport stuff to camera?
	Renderer::Camera::updateAllCamerasAspectRatio((float)(width/height));
	Gui::resize();
}
