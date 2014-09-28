#include "game.h"

Game::Game(std::string path)
{	
	Renderer::initialize();
	
	mContentFolderDir = "../content";
	mKnightModelRes = h3dAddResource(H3DResTypes::SceneGraph, "models/knight/knight.scene.xml", 0);
	// mKnightModelRes = h3dAddResource(H3DResTypes::SceneGraph, "models/test/test.scene.xml", 0);
	H3DRes envRes = h3dAddResource( H3DResTypes::SceneGraph, "models/sphere/sphere.scene.xml", 0 );
	mFontMatRes = h3dAddResource( H3DResTypes::Material, "overlays/font.material.xml", 0 );
	mPanelMatRes = h3dAddResource( H3DResTypes::Material, "overlays/panel.material.xml", 0 );
	h3dutLoadResourcesFromDisk(mContentFolderDir.c_str());

	mKnightNode = h3dAddNodes(H3DRootNode, mKnightModelRes);
	h3dSetNodeTransform(mKnightNode,
						0, 5, 0,
						0, 0, 0,
						1.f, 1.f, 1.f);

	H3DNode env = h3dAddNodes( H3DRootNode, envRes );
	h3dSetNodeTransform( env, 0, -20, 0, 0, 0, 0, 20, 20, 20 );
	
	// Add light source
	H3DNode light = h3dAddLightNode(H3DRootNode, "Light1", 0, "LIGHTING", "SHADOWMAP");
	h3dSetNodeTransform(light, 0, 15, 10, -60, 0, 0, 1, 1, 1);
	h3dSetNodeParamF(light, H3DLight::RadiusF, 0, 30);
	h3dSetNodeParamF(light, H3DLight::FovF, 0, 90);
	h3dSetNodeParamI(light, H3DLight::ShadowMapCountI, 1);
	h3dSetNodeParamF(light, H3DLight::ShadowMapBiasF, 0, 0.01f);
	h3dSetNodeParamF(light, H3DLight::ColorF3, 0, 1.0f);
	h3dSetNodeParamF(light, H3DLight::ColorF3, 1, 0.8f);
	h3dSetNodeParamF(light, H3DLight::ColorF3, 2, 0.7f);
	h3dSetNodeParamF(light, H3DLight::ColorMultiplierF, 0, 1.0f);


	GOPtr player = std::make_shared<GameObject>();
	player->setName("Player");
	Transform transform;
	player->addComponent(std::make_shared<Transform>(transform));
	Camera camera(player->getNode());
	player->addComponent(std::make_shared<Camera>(camera));
	player->setTag("FreeCamera");
	mObjectList.push_back(player);

	GOPtr suzanne = std::make_shared<GameObject>();
	suzanne->setName("Suzanne");
	Transform suzTransform;
	suzTransform.setPosition(glm::vec3(-2, 3, 0));
	suzanne->addComponent(std::make_shared<Transform>(suzTransform));
	Model suzModel(suzanne->getNode(), "models/test/test.scene.xml");
	suzanne->addComponent(std::make_shared<Model>(suzModel));
	mObjectList.push_back(suzanne);
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
}
