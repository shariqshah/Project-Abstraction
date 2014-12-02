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

Game::Game(std::string path)
{	
	Renderer::initialize(path);
	Renderer::Camera::initialize();
	System::initialize();
	
	GOPtr falcon = SceneManager::create("Falcon");
	falcon->tag = "Falcon";
	auto falMod = CompManager::addModel(falcon, "models/falcon/falcon.scene.xml");
	//auto falconTransform = CompManager::getTransform(falcon);
	CompManager::addRigidBody(falcon, new CollisionMesh(*falMod, false), 10);

	GOPtr lightGO = SceneManager::create("LightGO");
	CompManager::addModel(lightGO, "models/sphere/sphere.scene.xml");
	auto goLight = CompManager::addLight(lightGO, "GOLight");
	auto lTransform = CompManager::getTransform(lightGO);

	Transform::setPosition(lTransform, Vec3(-176, 319, 18));
	Transform::setLookAt(lTransform, Vec3(0));
	Renderer::Light::setColor(goLight, Vec3(0.8f, 0.8f, 0.5f));
	Renderer::Light::setRadius(goLight, 500);

	GOPtr player = SceneManager::create("Player");
	player->tag = "FreeCamera";
	auto playerTrans = CompManager::getTransform(player);
	auto playerLight = CompManager::addLight(player, "playerLight");
	CompManager::addCamera(player, "playerCamera");
	Renderer::Light::setColor(playerLight, Vec3(1, 0, 0));
	Transform::setPosition(playerTrans, Vec3(-110, 85, 92));
	System::CameraSystem::setActiveObject(player);
	GO::attachScript(player, "PlayerBehaviour");
	mCurrentViewer = player;

	Sphere* sphere = new Sphere(1);
	for(int i = 0; i < 10; i++)
	{
		GOPtr suzanne = SceneManager::create("Suzanne" +
													   std::to_string(i));
		suzanne->tag = "suzanne";
		auto suzTransform = CompManager::getTransform(suzanne);

		float radius = 20.f;
		float width  = glm::sin((float)i) * radius;
		float height = glm::cos((float)i) * radius;
		if(i < 50)
			Transform::setPosition(suzTransform, Vec3(width, 5, height));
		else
			Transform::setPosition(suzTransform, Vec3(0, height + 7, width));
		
		CompManager::addModel(suzanne, "models/test/test.scene.xml");
		CompManager::addRigidBody(suzanne, sphere);

		auto suzLight = CompManager::addLight(suzanne,
											  "light" + std::to_string(i));
		
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		Renderer::Light::setColor(suzLight, Vec3(r, g, b));
		Renderer::Light::setShadowCaster(suzLight, false);
	}

	GOPtr plane = SceneManager::create("Floor");
	CompManager::addRigidBody(plane, new Plane(Vec3(0, 1, 0), 0.04f), 0.f);

	Renderer::Camera::resizePipelineBuffers(Settings::getWindowWidth(),
											Settings::getWindowHeight());
}

Game::~Game()
{
	System::cleanup();
}

void Game::update(float deltaTime)
{
	System::update(deltaTime);
}

void Game::draw()
{
	auto activeCamera = CompManager::getCamera(mCurrentViewer);
	auto activeTrans  = CompManager::getTransform(mCurrentViewer);
	Renderer::renderFrame(activeCamera->node);
	Physics::draw(activeTrans, activeCamera);
}

void Game::resize(int width, int height)
{
	auto activeCamera = CompManager::getCamera(mCurrentViewer);
	Renderer::Camera::resizePipelineBuffers(width, height);
	Renderer::Camera::setViewportSize(activeCamera,
									  width,
									  height);
}
