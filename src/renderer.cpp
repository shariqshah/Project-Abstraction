#include "renderer.h"

namespace Renderer
{
	namespace
	{
		const std::string cContentFolderDir = "../content";
		static Resource sCurrentPipeline;
	}
	
	void syncTransform(glm::vec3 position,
					   glm::vec3 rotation,
					   glm::vec3 scale,
					   H3DNode node)
	{	
		h3dSetNodeTransform(node,						        // node
							position.x, position.y, position.z, // position
							rotation.x, rotation.y, rotation.z,	// rotation
							scale.x   , scale.y   , scale.z);	// scale
	}

	void initialize()
	{
		// Set options
		h3dSetOption(H3DOptions::LoadTextures, 1);
		h3dSetOption(H3DOptions::TexCompression, 0);
		h3dSetOption(H3DOptions::FastAnimation, 0);
		h3dSetOption(H3DOptions::MaxAnisotropy, 4);
		h3dSetOption(H3DOptions::ShadowMapSize, 2048);

		// Initialize Pipeline
		sCurrentPipeline = h3dAddResource(H3DResTypes::Pipeline, "pipelines/forward.pipeline.xml", 0);

		h3dutLoadResourcesFromDisk(cContentFolderDir.c_str());
	}

	Resource getCurrentPipline()
	{
		return sCurrentPipeline;
	}

	Node createCamera(std::string name, Node parent)
	{
		Node node = h3dAddCameraNode(parent, name.c_str(), sCurrentPipeline);
		return node;
	}
	
    Node createGroupNode(std::string name, Node parent)
	{
		Node node = h3dAddGroupNode(parent, name.c_str());
		return node;
	}

	void setNodeParam(Node node, int param, int value)
	{
		h3dSetNodeParamI(node, param, value);
	}
	
    void setNodeParam(Node node, int param, int compID, float value)
	{
		h3dSetNodeParamF(node, param, compID,  value);
	}

	void setCameraView(Node cameraNode,
					   float fov,
					   float aspect,
					   float nearZ,
					   float farZ)
	{
		h3dSetupCameraView(cameraNode, fov, aspect, nearZ, farZ);
	}

	void resizePipelineBuffers(int width, int height)
	{
		h3dResizePipelineBuffers(sCurrentPipeline, width, height);
	}
}
