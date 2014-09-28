#include "renderer.h"

namespace Renderer
{
	namespace
	{
		const  std::string cContentFolderDir = "../content";
		static std::vector<std::string> sTextList(10);
		static Resource sFontMat;
		static Resource sPanelMat;
		static Resource sCurrentPipeline;
		static Node sCurrentCamera;
		static glm::vec2 sFontPos;
		static float sFontSize;
	}

	void setNodeTransform(Node node, glm::mat4 transformMat)
	{
		h3dSetNodeTransMat(node, glm::value_ptr(transformMat));
	}
	
	void syncNodeTransform(glm::vec3 position,
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

		sCurrentPipeline = h3dAddResource(H3DResTypes::Pipeline, "pipelines/forward.pipeline.xml", 0);
		sFontMat = h3dAddResource(H3DResTypes::Material, "overlays/font.material.xml", 0);
		sPanelMat = h3dAddResource(H3DResTypes::Material, "overlays/panel.material.xml", 0);
		
		h3dutLoadResourcesFromDisk(cContentFolderDir.c_str());

		sFontPos = glm::vec2(0.03, 0.25);
		sFontSize = 0.026f;
	}

	Resource getCurrentPipline()
	{
		return sCurrentPipeline;
	}

	Node createCamera(std::string name, Node parent)
	{
		Node node = h3dAddCameraNode(parent, name.c_str(), sCurrentPipeline);

		if(node != 0)
			setCurrentCamera(node);
		
		return node;
	}
	
    Node createGroupNode(std::string name, Node parent)
	{
		Node node = h3dAddGroupNode(parent, name.c_str());
		return node;
	}

	Node createNode(Resource resource, Node parent)
	{
		Node node = h3dAddNodes(parent, resource);
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

	void resizePipelineBuffers(int width, int height)
	{
		h3dResizePipelineBuffers(sCurrentPipeline, width, height);
	}

	void setCurrentCamera(Node cameraNode)
	{
		sCurrentCamera = cameraNode;
	}

	Node getCurrentCameraNode()
	{
		return sCurrentCamera;
	}

	void drawText()
	{
		int count = 0;
		for(std::string text : sTextList)
		{
			h3dutShowText(text.c_str(),
						  sFontPos.x,
						  sFontPos.y + (sFontSize * count),
						  sFontSize,
						  1, 1, 1,
						  sFontMat);
			count++;
		}

		sTextList.clear();	
	}

	void addText(std::string text)
    {
		sTextList.push_back(text);
    }

	bool setParent(Node child, Node parent)
	{
		if(h3dSetNodeParent(child, parent))
			return true;
		else
			return false;
	}

	namespace Camera
	{
		void setViewportSize(Node camera, int width, int height)
		{
			h3dSetNodeParamI(camera, H3DCamera::ViewportWidthI, width);
			h3dSetNodeParamI(camera, H3DCamera::ViewportHeightI, height);
	    }

		void setViewportPos(Node camera, int x, int y)
		{
			h3dSetNodeParamI(camera, H3DCamera::ViewportXI, x);
			h3dSetNodeParamI(camera, H3DCamera::ViewportYI, y);
		}

		void setView(Node  cameraNode,
					 float fov,
					 float aspect,
					 float nearZ,
					 float farZ)
		{
			h3dSetupCameraView(cameraNode, fov, aspect, nearZ, farZ);
		}
	}

	namespace Resources
	{	
		bool loadAddedResources()
		{
			return h3dutLoadResourcesFromDisk(cContentFolderDir.c_str());
		}

		Resource add(ResourceType type, std::string name, int flag)
		{
			Resource res = h3dAddResource((int)type, name.c_str(), flag);
			return res;
		}
		
		Resource get(ResourceType type, std::string name)
		{
			Resource res = h3dFindResource((int)type, name.c_str());

			if(res == 0)
			{
				Log::message(name + " is not loaded yet. Loading now..");
				res = add(type, name);
				
				if(!loadAddedResources())
					Log::error(Log::ErrorLevel::LOW, name + " not found!");
				else
					Log::message(name + " has been loaded!");
			}

			return res;
		}
	}
}
