#include "cpu.h"
#include "camera.h"
#include "renderer.h"
#include "scenemanager.h"
#include "transform.h"
#include "model.h"

namespace Cpu
{
	namespace
	{
	}
	
	void initialize()
	{
		// auto model = CompManager::addModel(cpuGO, "models/terminal/terminal.scene.xml");

		// auto transform = CompManager::getTransform(cpuGO);
		// Transform::setPosition(transform, Vec3(-50, 0, 0));

		// int resource = Renderer::Resources::get(ResourceType::MATERIAL, "CPU_Screen_Mat.material.xml");

        // int mesh = h3dGetNodeChild(model->node, 1);
		// int material = h3dGetNodeParamI(mesh, H3DMesh::MatResI);
		// int samplerIndex = h3dFindResElem(material, H3DMatRes::SamplerElem, H3DMatRes::SampNameStr, "albedoMap");
		// h3dSetResParamI(material, H3DMatRes::SamplerElem, samplerIndex, H3DMatRes::SampTexResI, screenTexture);

		// test = SceneManager::create("CPU_Test_GO");
		// auto camera = CompManager::addCamera(test, "CPU_Camera", Pipeline::FORWARD);
		// // Camera::setOutputTexture(camera, screenTexture);
		// auto testTran = CompManager::getTransform(test);
		// Transform::setPosition(testTran, Vec3(-10, 0, 0));
	}
	
	void update(float deltaTime)
	{
		// auto transform = CompManager::getTransform(test);
		// Transform::rotate(transform, Vec3(0, 1, 0), 20 * deltaTime, Transform::Space::WORLD);
	}

	void draw()
	{
		// auto camera = CompManager::getCamera(test);
		// h3dRender(camera->node);
	}
	
	void cleanup()
	{
		
	}
	
}

