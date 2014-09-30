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
		static std::vector<Node> cameras(5, 0);
	}

	void setNodeTransform(Node node, glm::mat4 transformMat)
	{
		h3dSetNodeTransMat(node, glm::value_ptr(transformMat));
	}

	void setNodeName(Node node, const std::string name)
	{
		h3dSetNodeParamStr(node, H3DNodeParams::NameStr, name.c_str());
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
			cameras.push_back(node);
		
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

	void removeCamera(Node cameraToRemove)
	{
		if(!cameras.empty())
		{
			cameras.erase(std::find(cameras.begin(),
									cameras.end(),
									cameraToRemove),
						  cameras.end());

			if(cameraToRemove == sCurrentCamera)
			{
				if(!cameras.empty())
					sCurrentCamera = cameras.front();
				else
				{
					Log::warning("No active cameras left in the scene!");
					sCurrentCamera = 0;
				}
			}
		}
	    else
			Log::error(Log::ErrorLevel::MEDIUM,
					   "Could not remove camera. No cameras in scene!");
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

	bool removeNode(Node node)
	{
		if(node == 0)
			return false;
		else
			h3dRemoveNode(node);

		return true;
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

		bool remove(Resource resource)
		{
			if(h3dRemoveResource(resource) == -1)
				return false;

			return true;
		}

		bool isLoaded(Resource resource)
		{
			return h3dIsResLoaded(resource);
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

	namespace Light
	{
		Node create(Node parent,
					const std::string name,
					Resource material,
					const std::string lightContext,
					const std::string shadowContext)
		{
			Node light = h3dAddLightNode(parent,
										 name.c_str(),
										 material,
										 lightContext.c_str(),
										 shadowContext.c_str());

			if(light == 0)
				Log::error(Log::ErrorLevel::LOW, name + " node not created!");

			return light;
		}
		
		void setRadius(Node light, float radius)
		{
			h3dSetNodeParamF(light, H3DLight::RadiusF, 0, radius);
		}
		
		void setColor(Node light, glm::vec3 color)
		{
			h3dSetNodeParamF(light, H3DLight::ColorF3, 0, color.r);
			h3dSetNodeParamF(light, H3DLight::ColorF3, 1, color.g);
			h3dSetNodeParamF(light, H3DLight::ColorF3, 2, color.b);
		}
		
		void setFov(Node light, float fov)
		{
			h3dSetNodeParamF(light, H3DLight::FovF, 0, fov);
		}
		
		void setShadowMapCount(Node light, int count)
		{
			h3dSetNodeParamI(light, H3DLight::ShadowMapCountI, count);
		}
		
		void setShadowMapBias(Node light, float bias)
		{
			h3dSetNodeParamF(light, H3DLight::ShadowMapBiasF, 0, bias);
		}

		void setShadowSplitLambda(Node light, float splitLambda)
		{
			h3dSetNodeParamF(light, H3DLight::ShadowSplitLambdaF, 0, splitLambda);
		}
		
		void setIntensity(Node light, float intensity)
		{
			h3dSetNodeParamF(light, H3DLight::ColorMultiplierF, 0, intensity);
		}
	}
}
