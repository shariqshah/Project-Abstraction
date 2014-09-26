#include "game.h"

Game::Game(std::string path)
{	
	Renderer::initialize();
	
	mContentFolderDir = "../content";
	// mForwardPipeRes = h3dAddResource(H3DResTypes::Pipeline, "pipelines/forward.pipeline.xml", 0);
	mKnightModelRes = h3dAddResource(H3DResTypes::SceneGraph, "models/knight/knight.scene.xml", 0);
	H3DRes envRes = h3dAddResource( H3DResTypes::SceneGraph, "models/sphere/sphere.scene.xml", 0 );
	mFontMatRes = h3dAddResource( H3DResTypes::Material, "overlays/font.material.xml", 0 );
	mPanelMatRes = h3dAddResource( H3DResTypes::Material, "overlays/panel.material.xml", 0 );
	mLogoMatRes = h3dAddResource( H3DResTypes::Material, "overlays/logo.material.xml", 0 );
	h3dutLoadResourcesFromDisk(mContentFolderDir.c_str());

	mKnightNode = h3dAddNodes(H3DRootNode, mKnightModelRes);
	h3dSetNodeTransform(mKnightNode,
						0, 0, 0,
						0, 0, 0,
						0.1f, 0.1f, 0.1f);

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

	// std::string str2 = std::to_string(Input::getMouseX()) + ", " +
	// 	               std::to_string(Input::getMouseY());
	
	// h3dutShowText( str2.c_str(), 0.03f, 0.27f, 0.026f, 1, 1, 1, mFontMatRes );

	// std::string str3 = std::to_string(Input::getMouseRelX()) + ", " +
	// 	                  std::to_string(Input::getMouseRelY());           
	
	// h3dutShowText( str3.c_str(), 0.03f, 0.30f, 0.026f, 1, 1, 1, mFontMatRes );

	Renderer::drawText();
	
	h3dRender(Renderer::getCurrentCameraNode());
	h3dFinalizeFrame();
	h3dClearOverlays();
	h3dutDumpMessages();
}

void Game::resize(int width, int height)
{
	// H3DNode camNode = mCamera->getCameraNode();

	// // Resize viewport
	// h3dSetNodeParamI(camNode, H3DCamera::ViewportXI, 0);
	// h3dSetNodeParamI(camNode, H3DCamera::ViewportYI, 0);
	// h3dSetNodeParamI(camNode, H3DCamera::ViewportWidthI, width);
	// h3dSetNodeParamI(camNode, H3DCamera::ViewportHeightI, height);
	
	// // Set virtual camera parameters
	// h3dSetupCameraView(camNode, 45.0f, (float)width / height, 0.1f, 1000.0f);
	// h3dResizePipelineBuffers(mForwardPipeRes, width, height);
}
