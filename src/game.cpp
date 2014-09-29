#include "game.h"

Game::Game(std::string path)
{	
	Renderer::initialize();
	
	mKnightModelRes = Renderer::Resources::add(ResourceType::MODEL,
											   "models/buddha/buddha.scene.xml");
	H3DRes envRes = Renderer::Resources::add(ResourceType::MODEL,
											 "models/sphere/sphere.scene.xml");
	mFontMatRes = Renderer::Resources::add(ResourceType::MATERIAL,
										   "overlays/font.material.xml");
	mPanelMatRes = Renderer::Resources::add(ResourceType::MATERIAL,
											"overlays/panel.material.xml");
	Renderer::Resources::loadAddedResources();

	mKnightNode = h3dAddNodes(H3DRootNode, mKnightModelRes);
	h3dSetNodeTransform(mKnightNode,
						0, 2, 0,
						0, 0, 0,
						1.f, 1.f, 1.f);

	H3DNode env = h3dAddNodes(H3DRootNode, envRes);
	h3dSetNodeTransform(env, 0, -20, 0, 0, 0, 0, 20, 20, 20);
	
	// Add light source
	// H3DNode light = h3dAddLightNode(H3DRootNode, "Light1", 0, "LIGHTING", "SHADOWMAP");
	// h3dSetNodeTransform(light, 0, 20, 16, -60, 0, 0, 1, 1, 1);
	// h3dSetNodeParamF(light, H3DLight::RadiusF, 0, 30);
	// h3dSetNodeParamF(light, H3DLight::FovF, 0, 90);
	// h3dSetNodeParamI(light, H3DLight::ShadowMapCountI, 1);
	// h3dSetNodeParamF(light, H3DLight::ShadowMapBiasF, 0, 0.01f);
	// h3dSetNodeParamF(light, H3DLight::ColorF3, 0, 1.0f);
	// h3dSetNodeParamF(light, H3DLight::ColorF3, 1, 0.8f);
	// h3dSetNodeParamF(light, H3DLight::ColorF3, 2, 0.7f);
	// h3dSetNodeParamF(light, H3DLight::ColorMultiplierF, 0, 1.0f);

	GOPtr lightGO = std::make_shared<GameObject>();
	lightGO->setName("LightGO");
	Transform lTransform;
	lTransform.setPosition(glm::vec3(-2, 15, 15));
	//lTransform.setLookAt(glm::vec3(0.f));
	//lTransform.rotate(glm::vec3(1, 0, 0), 90);
	Model monkey(lightGO->getNode(), "models/sphere/sphere.scene.xml");
	lightGO->addComponent(std::make_shared<Model>(monkey));
	lightGO->addComponent(std::make_shared<Transform>(lTransform));
	Light goLight(lightGO->getNode(), "GOLight");
	goLight.setColor(glm::vec3(0, 0, 1));
	lightGO->addComponent(std::make_shared<Light>(goLight));
	mObjectList.push_back(lightGO);

	GOPtr player = std::make_shared<GameObject>();
	player->setName("Player");
	Transform transform;
	player->addComponent(std::make_shared<Transform>(transform));
	Camera camera(player->getNode());
	player->addComponent(std::make_shared<Camera>(camera));
	// Light camLight(player->getNode(), "newLight");
	// camLight.setColor(glm::vec3(1, 0, 0));
	// player->addComponent(std::make_shared<Light>(camLight));
	player->setTag("FreeCamera");
	mObjectList.push_back(player);

	for(int i = 0; i < 50; i++)
	{
		GOPtr suzanne = std::make_shared<GameObject>();
		suzanne->setName("Suzanne" + std::to_string(i));
		Transform suzTransform;

		float radius = 7.f;
		float width  = glm::sin((float)i) * radius;
		float height = glm::cos((float)i) * radius;
		if(i < 50)
			suzTransform.setPosition(glm::vec3(width, 5, height));
		else
			suzTransform.setPosition(glm::vec3(0, height + 7, width));
		
		suzanne->addComponent(std::make_shared<Transform>(suzTransform));
		Model suzModel(suzanne->getNode(), "models/test/test.scene.xml");
		suzanne->addComponent(std::make_shared<Model>(suzModel));
		mObjectList.push_back(suzanne);
	}

	Renderer::resizePipelineBuffers(Settings::getWindowWidth(),
									Settings::getWindowHeight());
}

void Game::update(float deltaTime)
{
	for(GOPtr gameObject : mObjectList)
	{
		System::update(deltaTime, gameObject.get());
	}
}

void Game::draw()
{
	h3dutShowFrameStats( mFontMatRes, mPanelMatRes, 1);

	Renderer::drawText();
	
	h3dRender(Renderer::getCurrentCameraNode());
	h3dFinalizeFrame();
	h3dClearOverlays();
	h3dutDumpMessages();
}

void Game::resize(int width, int height)
{
	Renderer::resizePipelineBuffers(width, height);
	Renderer::Camera::setViewportSize(Renderer::getCurrentCameraNode(),
									  width,
									  height);
}
