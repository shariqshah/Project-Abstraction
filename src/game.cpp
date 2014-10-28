#include "game.h"

Game::Game(std::string path)
{	
	Renderer::initialize(path);
	System::initialize();
	
	GOPtr sponza = SceneManager::createGameObject("Sponza");
	auto sponMod = sponza->addComponent<Model>(sponza->getNode(),
											   "models/terrain/terrain.scene.xml");
	auto sponTran = sponza->getComponent<Transform>();
	sponza->addComponent<RigidBody>(sponTran,
									new CollisionMesh(sponMod, true), 0.f);
	//sponTran->setScale(glm::vec3(20, 20, 20));
	
	GOPtr falcon = SceneManager::createGameObject("Falcon");
	auto falMod = falcon->addComponent<Model>(falcon->getNode(),
											  "models/falcon/falcon.scene.xml");
	auto falconTransform = falcon->getComponent<Transform>();
    falcon->addComponent<RigidBody>(falconTransform,
									new CollisionMesh(falMod, false), 5.f);
	//falconTransform->setScale(glm::vec3(3, 3, 3));

	GOPtr lightGO = SceneManager::createGameObject("LightGO");
	lightGO->addComponent<Model>(lightGO->getNode(),
								 "models/sphere/sphere.scene.xml");
	auto goLight = lightGO->addComponent<Light>(lightGO->getNode(), "GOLight");
	auto lTransform = lightGO->getComponent<Transform>();

	lTransform->setPosition(glm::vec3(-176, 319, 18));
	lTransform->setLookAt(glm::vec3(0));
	goLight->setColor(glm::vec3(0.8f, 0.8f, 0.5f));
	goLight->setRadius(500);

	GOPtr player = SceneManager::createGameObject("Player");
	player->setTag("FreeCamera");
	auto playerTrans = player->getComponent<Transform>();
	auto camera = player->addComponent<Camera>(player->getNode());
	playerTrans->setPosition(glm::vec3(-110, 85, 92));
	//playerTrans->setLookAt(glm::vec3(0));
	Renderer::setCurrentCamera(camera->getCameraNode());

	Sphere* sphere = new Sphere(1);
	for(int i = 0; i < 10; i++)
	{
		GOPtr suzanne = SceneManager::createGameObject("Suzanne" + std::to_string(i));
		auto suzTransform = suzanne->getComponent<Transform>();

		float radius = 20.f;
		float width  = glm::sin((float)i) * radius;
		float height = glm::cos((float)i) * radius;
		if(i < 50)
			suzTransform->setPosition(glm::vec3(width, 5, height));
		else
			suzTransform->setPosition(glm::vec3(0, height + 7, width));
		
		suzanne->addComponent<Model>(suzanne->getNode(),
									 "models/test/test.scene.xml");

		suzanne->addComponent<RigidBody>(suzTransform, sphere);
		suzanne->setTag("suzanne");
		// auto sLight = suzanne->addComponent<Light>(suzanne->getNode(),
		// 										   "light" + std::to_string(i));
		// float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		// float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		// float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		// sLight->setColor(glm::vec3(r, g, b));
		// sLight->setShadowCaster(false);
	}

	GOPtr plane = SceneManager::createGameObject("Floor");
	plane->addComponent<RigidBody>(plane->getComponent<Transform>(),
								   new Plane(glm::vec3(0, 1, 0), 0.04f), 0.f);

	Renderer::resizePipelineBuffers(Settings::getWindowWidth(),
									Settings::getWindowHeight());
}

Game::~Game()
{
	SceneManager::cleanup();
	System::cleanup();
}

void Game::update(float deltaTime)
{		
	System::update(deltaTime);
}

void Game::draw()
{
	Renderer::renderFrame();
}

void Game::resize(int width, int height)
{
	Renderer::resizePipelineBuffers(width, height);
	Renderer::Camera::setViewportSize(Renderer::getCurrentCameraNode(),
									  width,
									  height);
}
