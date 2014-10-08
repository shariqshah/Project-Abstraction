#include "game.h"

Game::Game(std::string path)
{	
	Renderer::initialize();

	GOPtr sponza = SceneManager::createGameObject("Sponza");
	sponza->addComponent<Model>(sponza->getNode(),
								"models/sponza/sponza.scene.xml");
	
	GOPtr falcon = SceneManager::createGameObject("Falcon");
	falcon->addComponent<Model>(falcon->getNode(),
								"models/falcon/falcon.scene.xml");

	GOPtr lightGO = SceneManager::createGameObject("LightGO");
	lightGO->addComponent<Model>(lightGO->getNode(),
								 "models/sphere/sphere.scene.xml");
	auto goLight = lightGO->addComponent<Light>(lightGO->getNode(), "GOLight");
	auto lTransform = lightGO->getComponent<Transform>();

	lTransform->setPosition(glm::vec3(-2, 15, 15));
	goLight->setColor(glm::vec3(0, 0, 1));

	GOPtr player = SceneManager::createGameObject("Player");
	player->setTag("FreeCamera");
	auto camera = player->addComponent<Camera>(player->getNode());
	Renderer::setCurrentCamera(camera->getCameraNode());

	for(int i = 0; i < 0; i++)
	{
		GOPtr suzanne = SceneManager::createGameObject("Suzanne" + std::to_string(i));
		auto suzTransform = suzanne->getComponent<Transform>();

		float radius = 7.f;
		float width  = glm::sin((float)i) * radius;
		float height = glm::cos((float)i) * radius;
		if(i < 50)
			suzTransform->setPosition(glm::vec3(width, 5, height));
		else
			suzTransform->setPosition(glm::vec3(0, height + 7, width));
		
		suzanne->addComponent<Model>(suzanne->getNode(), "models/test/test.scene.xml");
	}

	Renderer::resizePipelineBuffers(Settings::getWindowWidth(),
									Settings::getWindowHeight());
}

Game::~Game()
{
	SceneManager::cleanup();
}

void Game::update(float deltaTime)
{
	SceneObjectMap* sceneObjects = SceneManager::getSceneObjects();
	for(SceneObjectMap::iterator it = sceneObjects->begin();
		it != sceneObjects->end();
		++it)
	{
		System::update(deltaTime, it->second.get());
	}

	SceneManager::update();
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
