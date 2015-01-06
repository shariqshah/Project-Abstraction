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
#include "componentmanager.h"
#include "camerasystems.h"
#include "cpu.h"
#include "utilities.h"
#include "shader.h"
#include "texture.h"

Game::Game(const char* path)
{
	Renderer::initialize(path);

	float length = 1.f;
	float height = 1.f;
	float width  = 1.f;

	length /= 2.f;
	height /= 2.f;
	width  /= 2.f;
	
	    //Vertices
    std::vector<Vec3>vertices;
    vertices.push_back(Vec3(-length,  height, width));
    vertices.push_back(Vec3(-length, -height, width));
    vertices.push_back(Vec3( length, -height, width));
    vertices.push_back(Vec3( length,  height, width));
	
    vertices.push_back(Vec3(-length,  height, -width));
    vertices.push_back(Vec3(-length, -height, -width));
    vertices.push_back(Vec3( length, -height, -width));
    vertices.push_back(Vec3( length,  height, -width));	

    //UV's
    std::vector<Vec2>uvs;
    uvs.push_back(Vec2(0.0, 0.0));
    uvs.push_back(Vec2(1.0, 0.0));
    uvs.push_back(Vec2(1.0, 1.0));
    uvs.push_back(Vec2(1.0, 1.0));
    uvs.push_back(Vec2(0.0, 1.0));
    uvs.push_back(Vec2(0.0, 0.0));

	std::vector<unsigned int> indices;

	//front
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(2); indices.push_back(3); indices.push_back(0);

	//back
	indices.push_back(6); indices.push_back(5); indices.push_back(4);
	indices.push_back(4); indices.push_back(7); indices.push_back(6);

	//left
	indices.push_back(4); indices.push_back(5); indices.push_back(1);
	indices.push_back(1); indices.push_back(0); indices.push_back(4);

	//right
	indices.push_back(3); indices.push_back(2); indices.push_back(6);
	indices.push_back(6); indices.push_back(7); indices.push_back(3);

	//top
	indices.push_back(4); indices.push_back(0); indices.push_back(3);
	indices.push_back(3); indices.push_back(7); indices.push_back(4);

	//bottom
	indices.push_back(1); indices.push_back(5); indices.push_back(6);
	indices.push_back(6); indices.push_back(2); indices.push_back(1);

	std::vector<Vec3> triVerts;
	triVerts.push_back(Vec3(-0.5, -0.5, 0));
	triVerts.push_back(Vec3( 0.5, -0.5, 0));
	triVerts.push_back(Vec3( 0,    0.5, 0));

	std::vector<unsigned int> triIndices;
	triIndices.push_back(0);
	triIndices.push_back(1);
	triIndices.push_back(2);

	CModel triModel;
	triModel.filename = "TriMesh";
	triModel.vertices = triVerts;
	triModel.uvs      = uvs;
	// triModel.indices  = triIndices;
	triModel.material = MAT_UNSHADED;
	
	CModel model;
	model.filename = "CubeMesh";
	model.vertices = vertices;
	model.uvs      = uvs;
	model.indices  = indices;
	model.material = MAT_UNSHADED;
	model.materialUniforms.diffuseColor = Vec3(1, 0, 0);
	
	GameObject* triangle = SceneManager::create("Triangle");
	GO::addModel(triangle, &triModel);
	CTransform* modelTransform = GO::getTransform(triangle);
	Transform::setPosition(modelTransform, Vec3(-5, 0, -5), true);

	GameObject* cube = SceneManager::create("Cube");
	GO::addModel(cube, &model);
	CTransform* cubeTran = GO::getTransform(cube);
	Transform::setPosition(cubeTran, Vec3(5, 0, -5), true);
	
	for(int i = 0; i < 10; i++)
	{
		GameObject* cube2 = SceneManager::create("Cube" + std::to_string(i));
		model.materialUniforms.diffuseColor = Vec3(1/(i + 1), 0, 0);
		GO::addModel(cube2, &model);
		CTransform* cube2Tran = GO::getTransform(cube2);
		Transform::setPosition(cube2Tran, Vec3(i, i, -i), true);
		//Transform::setScale(cube2Tran, Vec3(2, 2, 2));
	}
	
	GameObject* playerPtr = SceneManager::create("Player");
	player = playerPtr->node;
	playerPtr->tag = "FreeCamera";
	CTransform* transform = GO::getTransform(playerPtr);
	Transform::translate(transform, Vec3(0, 0, 5));

	GO::addCamera(playerPtr);
	System::CameraSystem::setActiveObject(playerPtr);

	TextRect text;
	text.text = "Hello";
	Renderer::addTextRect(text);
}

Game::~Game()
{
	System::cleanup();
	Renderer::cleanup();
}

void Game::update(float deltaTime, bool* quit)
{
	auto cube = SceneManager::find("Cube2");
	auto tran = GO::getTransform(cube);
	Transform::rotate(tran, Vec3(0, 1, 0), 30 * deltaTime);
	
	System::update(deltaTime, quit);
}

void Game::draw()
{
	GameObject* playerPtr = SceneManager::find(player);
	CCamera* camera = Renderer::Camera::getCameraAtIndex(playerPtr->compIndices[(int)Component::CAMERA]);
	Renderer::renderFrame(camera);
	
	// Physics::draw(activeTrans, activeCamera);
}

void Game::resize(int width, int height)
{
	glViewport(0, 0, width, height);
	Renderer::Camera::updateAllCamerasAspectRatio((float)(width/height));
}
