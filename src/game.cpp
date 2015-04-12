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

	{
		GameObject* cube = SceneManager::create("Cube");
		CModel* cubeModel = GO::addModel(cube, "suzanne.pamesh");
		Model::setMaterialType(cubeModel, MAT_PHONG);
		cubeModel->materialUniforms.diffuseColor = Vec4(0, 1, 0, 1);
		cubeModel->materialUniforms.texture = Texture::create("layingrock.png");
		CTransform* cubeTran = GO::getTransform(cube);
		Transform::setPosition(cubeTran, Vec3(5, 0, -5), true);
		ScriptEngine::addScript(cube, "test");
	}

	Box* t1 = new Box(Vec3(0.5f));
	for(int i = 0; i < 10; i++)
	{
		GameObject* cube2 = SceneManager::create("Cube" + std::to_string(i));
		CModel* cubeModel = GO::addModel(cube2, "suzanne.pamesh");
		Model::setMaterialType(cubeModel, MAT_PHONG);
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
		Transform::setPosition(cube2Tran, Vec3(i, i + 5, -i), true);
		GO::addRigidbody(cube2, t1);
	}
	
	GameObject* playerPtr = SceneManager::create("Player");
	player = playerPtr->node;
	playerPtr->tag = "FreeCamera";
	CTransform* transform = GO::getTransform(playerPtr);
	Transform::translate(transform, Vec3(0, 3, 5));
	CCamera* camera = GO::addCamera(playerPtr);
	Camera::setActiveCamera(camera);
	//System::CameraSystem::setActiveObject(playerPtr);
	ScriptEngine::addScript(playerPtr, "freecamera");

	GameObject* plane = SceneManager::create("Ground");
	CModel* planeModel = GO::addModel(plane, "plane.pamesh");
    Model::setMaterialType(planeModel, MAT_PHONG);
	planeModel->materialUniforms.texture = Texture::create("test2.png");
	GO::addRigidbody(plane, new Plane(Vec3(0, 1, 0), 1.f), 0.f, 1.f);
	// ScriptEngine::addScript(plane, "test");

	GameObject* tea = SceneManager::create("Teapot");
	CModel* teapot = GO::addModel(tea, "teapot.pamesh");
	Model::setMaterialType(teapot, MAT_PHONG);
	teapot->materialUniforms.specularStrength = 60.f;
	teapot->materialUniforms.texture = Texture::create("chessboard.png");
	CTransform* tTran = GO::getTransform(tea);
	Transform::setPosition(tTran, Vec3(0, 10, -5), true);
	GO::addRigidbody(tea, new CollisionMesh(teapot, true), 0, 0.2f);
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
