#include "renderer.h"

namespace Renderer
{
	namespace
	{
		const  std::string cContentFolderDir = "../content";
		static std::vector<std::string> sTextList;
		static Resource sFontMat;
		static Resource sPanelMat;
		static Resource sDefaultPipeline;
		static Node sCurrentCamera;
		static glm::vec2 sFontPos;
		static float sFontSize;
		static std::vector<Node> cameras;
		static Resource sPipelines[3];
		static Resource sLightMat;
		static DebugLevel sDebugLevel;
		static bool sRenderWireframe;
		static bool sRenderDebugView;
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
		h3dSetOption(H3DOptions::ShadowMapSize, 1024);

		sPipelines[0] = Resources::add(ResourceType::PIPELINE,
									   "pipelines/forward.pipeline.xml",
									   0);
		sPipelines[1] = Resources::add(ResourceType::PIPELINE,
									   "pipelines/deferred.pipeline.xml",
									   0);
		sPipelines[2] = Resources::add(ResourceType::PIPELINE,
									   "pipelines/hdr.pipeline.xml",
									   0);

		sDefaultPipeline = sPipelines[(int)Pipeline::FORWARD];
		
		sFontMat = Resources::add(ResourceType::MATERIAL,
								  "overlays/font.material.xml",
								  0);
		sPanelMat = Resources::add(ResourceType::MATERIAL,
								   "overlays/panel.material.xml",
								   0);
		sLightMat = Resources::add(ResourceType::MATERIAL,
								   "materials/light.material.xml",
								   0);
		
		Resources::loadAddedResources();

		sFontPos = glm::vec2(0.03, 0.25);
		sFontSize = 0.026f;
		setDebugLevel(DebugLevel::MEDIUM);
		sRenderWireframe = sRenderDebugView = false;

		h3dSetOption(H3DOptions::DebugViewMode, sRenderDebugView ? 1.0f : 0.0f);
		h3dSetOption(H3DOptions::WireframeMode, sRenderWireframe ? 1.0f : 0.0f);
	}

	void setDebugLevel(DebugLevel level)
	{
		sDebugLevel = level;

		if(level == DebugLevel::NONE)
			Log::message("Renderer debug level set to NONE");
		if(level == DebugLevel::MEDIUM)
			Log::message("Renderer debug level set to MEDIUM");
		if(level == DebugLevel::HIGH)
			Log::message("Renderer debug level set to HIGH");
	}

	void toggleDebugView()
	{
		if(sRenderDebugView)
			sRenderDebugView = false;
		else
			sRenderDebugView = true;
		
		h3dSetOption(H3DOptions::DebugViewMode, sRenderDebugView ? 1.0f : 0.0f);
	}

	void toggleWireframe()
	{
		if(sRenderWireframe)
			sRenderWireframe = false;
		else
			sRenderWireframe = true;
		
		h3dSetOption(H3DOptions::WireframeMode, sRenderWireframe ? 1.0f : 0.0f);
	}

	void renderFrame()
	{
		if(sDebugLevel != DebugLevel::NONE)
		{
			if(sDebugLevel == DebugLevel::MEDIUM)
				h3dutShowFrameStats( sFontMat, sPanelMat, 1);
			else if(sDebugLevel == DebugLevel::HIGH)
				h3dutShowFrameStats( sFontMat, sPanelMat, 2);
			
			drawText();
		}
		else
			sTextList.clear();
		

		h3dRender(sCurrentCamera);
		h3dFinalizeFrame();
		h3dClearOverlays();
		h3dutDumpMessages();
	}

	Node createCamera(std::string name, Node parent)
	{
		Node node = h3dAddCameraNode(parent, name.c_str(), sDefaultPipeline);
		
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
		h3dResizePipelineBuffers(sPipelines[(int)Pipeline::FORWARD], width, height);
		h3dResizePipelineBuffers(sPipelines[(int)Pipeline::DEFERRED], width, height);
		h3dResizePipelineBuffers(sPipelines[(int)Pipeline::HDR], width, height);
	}

	void setCurrentCamera(Node cameraNode)
	{
		sCurrentCamera = cameraNode;
	}
	
	Node getParent(Node node)
	{
		Node parent = h3dGetNodeParent(node);
		return parent;
	}

	bool getNodeChildren(Node node, const std::string& name, NodeArray* children)
	{
		int childCount = h3dFindNodes(node, name.c_str(), H3DNodeTypes::Group);

		if(childCount > 1)
		{
			for(int i = 1; i < childCount; i++)
				children->push_back(h3dGetNodeFindResult(i));
		}

		return childCount > 1 ? true : false;
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

		void setPipeline(Node camera, Pipeline pipeline)
		{
			Resource newPipeline = sDefaultPipeline;
			
			switch(pipeline)
			{
			case Pipeline::FORWARD:
				newPipeline = sPipelines[(int)Pipeline::FORWARD];
				break;
			case Pipeline::DEFERRED:
				newPipeline = sPipelines[(int)Pipeline::DEFERRED];
				break;
			case Pipeline::HDR:
				newPipeline = sPipelines[(int)Pipeline::HDR];
				break;
			}

			h3dSetNodeParamI(camera, H3DCamera::PipeResI, newPipeline);
		}

		void setOcclusionCulling(Node camera, bool enable)
		{
			h3dSetNodeParamI(camera, H3DCamera::OccCullingI, enable ? 1 : 0);
		}

		void setOrthgraphic(Node camera, bool enable)
		{
			h3dSetNodeParamI(camera, H3DCamera::OrthoI, enable ? 1 : 0);
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

		bool setUniform(Resource material,
						const std::string& name,
						glm::vec4 value)
		{
			bool success = h3dSetMaterialUniform(material,
												 name.c_str(),
												 value.x,
												 value.y,
												 value.z,
												 value.w);
			if(!success)
			{
				Log::error(Log::ErrorLevel::LOW,
						   "could not set value for uniform " + name);
				return false;
			}

			return true;
			
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
										 sLightMat,
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
