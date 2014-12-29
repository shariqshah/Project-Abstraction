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
	
	    //Vertices
    std::vector<Vec3>vertices;
    vertices.push_back(Vec3(-0.5, -0.5, 0.0));
    vertices.push_back(Vec3( 0.5, -0.5, 0.0));
    vertices.push_back(Vec3(   0,  0.5, 0.0));
	
    vertices.push_back(Vec3(-1.0,  1.0, 0.0));
    vertices.push_back(Vec3(-1.0, -1.0, 0.0));
    vertices.push_back(Vec3( 1.0, -1.0, 0.0));

    //UV's
    std::vector<glm::vec2>uvs;
    uvs.push_back(glm::vec2(0.0, 0.0));
    uvs.push_back(glm::vec2(1.0, 0.0));
    uvs.push_back(glm::vec2(1.0, 1.0));
    uvs.push_back(glm::vec2(1.0, 1.0));
    uvs.push_back(glm::vec2(0.0, 1.0));
    uvs.push_back(glm::vec2(0.0, 0.0));

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	GLuint vertexVBO;
	glGenBuffers(1, &vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER,
				 vertices.size() * sizeof(Vec3),
				 vertices.data(),
				 GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint uvVBO;
	glGenBuffers(1, &uvVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER,
				 uvs.size() * sizeof(glm::vec2),
				 uvs.data(),
				 GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, 0);

	glBindVertexArray(0);

	mShader = Shader::create("../content/shaders/simple.vert",
							 "../content/shaders/simple.frag");

	mTexture = Texture::create("test.png");
	// Renderer::initialize(path);
	// Renderer::Camera::initialize();
	// System::initialize();
	
	// GOPtr falcon = SceneManager::create("Falcon");
	// falcon->tag = "Falcon";
	// auto falMod = CompManager::addModel(falcon, "models/falcon/falcon.scene.xml");
	// //auto falconTransform = CompManager::getTransform(falcon);
	// CompManager::addRigidBody(falcon, new CollisionMesh(falMod, false), 10);

	// GOPtr lightGO = SceneManager::create("LightGO");
	// CompManager::addModel(lightGO, "models/sphere/sphere.scene.xml");
	// auto goLight = CompManager::addLight(lightGO, "GOLight");
	// auto lTransform = CompManager::getTransform(lightGO);


	// Transform::setPosition(lTransform, Vec3(-50, 200, 0));
	// Transform::rotate(lTransform, Transform::UNIT_X, -90, Transform::Space::WORLD);
	// Renderer::Light::setColor(goLight, Vec3(0.8f, 0.8f, 0.5f));
	// Renderer::Light::setRadius(goLight, 500);

	// GOPtr player = SceneManager::create("Player");
	// player->tag = "FreeCamera";
	// auto playerTrans = CompManager::getTransform(player);
	// // auto playerLight = CompManager::addLight(player, "playerLight");
	// // Renderer::Light::setColor(playerLight, Vec3(1, 0, 0));
	// CompManager::addCamera(player, "playerCamera", Pipeline::FORWARD);
	// Transform::setPosition(playerTrans, Vec3(-18, 40, 28));
	// System::CameraSystem::setActiveObject(player);
	// GO::attachScript(player, "PlayerBehaviour");
	// mCurrentViewer = player;

	// Sphere* sphere = new Sphere(1);
	// for(int i = 0; i < 10; i++)
	// {
	// 	GOPtr suzanne = SceneManager::create("Suzanne" + std::to_string(i));
	// 	suzanne->tag = "suzanne";
	// 	auto suzTransform = CompManager::getTransform(suzanne);

	// 	float radius = 20.f;
	// 	float width  = glm::sin((float)i) * radius;
	// 	float height = glm::cos((float)i) * radius;
	// 	if(i < 50)
	// 		Transform::setPosition(suzTransform, Vec3(width, 5, height));
	// 	else
	// 		Transform::setPosition(suzTransform, Vec3(0, height + 7, width));
		
	// 	CompManager::addModel(suzanne, "models/test/test.scene.xml");
	// 	CompManager::addRigidBody(suzanne, sphere);

	// }

	// GOPtr plane = SceneManager::create("Floor");
	// CompManager::addRigidBody(plane, new Plane(Vec3(0, 1, 0), 0.04f), 0.f);

	// Renderer::Camera::resizePipelineBuffers(Settings::getWindowWidth(),
	// 										Settings::getWindowHeight());
}

Game::~Game()
{
	System::cleanup();
	Renderer::cleanup();
}

void Game::update(float deltaTime, bool* quit)
{
	System::update(deltaTime, quit);
}

void Game::draw()
{
	// auto activeCamera = CompManager::getCamera(mCurrentViewer);
	// auto activeTrans  = CompManager::getTransform(mCurrentViewer);
	// Cpu::draw();
	Renderer::renderFrame();

	if(mShader != -1)
	{
		Shader::bindShader(mShader);
		Texture::bindTexture(mTexture);
		glBindVertexArray(mVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 3, 3);
	    glBindVertexArray(0);
		Texture::unBindActiveTexture();
		Shader::unbindActiveShader();
	}
	
	// Physics::draw(activeTrans, activeCamera);
}

void Game::resize(int width, int height)
{
	// auto activeCamera = CompManager::getCamera(mCurrentViewer);
	// Renderer::Camera::resizePipelineBuffers(width, height);
	// Renderer::Camera::setViewportSize(activeCamera,
	// 								  width,
	// 								  height);
	glViewport(0, 0, width, height);
}
