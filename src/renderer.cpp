#include "renderer.h"

namespace Renderer
{
	namespace
	{
		std::string              cContentFolderDir;
		std::vector<std::string> sTextList;
		Vec2       sFontPos;
		Resource   sFontMat;
		Resource   sPanelMat;
		float      sFontSize;
		Resource   sLightMat;
		DebugLevel sDebugLevel;
		bool       sRenderWireframe;
		bool       sRenderDebugView;
	}

	void setNodeTransform(Node node, glm::mat4 transformMat)
	{
		h3dSetNodeTransMat(node, glm::value_ptr(transformMat));
	}

	void setNodeName(Node node, const std::string& name)
	{
		h3dSetNodeParamStr(node, H3DNodeParams::NameStr, name.c_str());
	}
	
	void setNodeTransform(Node node,
						  const Vec3 position,
						  const Vec3 rotation,
						  const Vec3 scale)
	{	
		h3dSetNodeTransform(node,						        
							position.x, position.y, position.z,
							rotation.x, rotation.y, rotation.z,
							scale.x   , scale.y   , scale.z);	
	}

	void getNodeTransform(Node node,
						  Vec3* position,
						  Vec3* rotation,
						  Vec3* scale)
	{
		h3dGetNodeTransform(node,						       
							&position->x, &position->y, &position->z,
							&rotation->x, &rotation->y, &rotation->z,
							&scale->x   , &scale->y   , &scale->z);
	}

	void initialize(const std::string& path)
	{
		cContentFolderDir = path + "../content";
		
		// Set options
		h3dSetOption(H3DOptions::LoadTextures  ,  1);
		h3dSetOption(H3DOptions::TexCompression,  0);
		h3dSetOption(H3DOptions::FastAnimation ,  0);
		h3dSetOption(H3DOptions::MaxAnisotropy ,  4);
		h3dSetOption(H3DOptions::ShadowMapSize ,  1024);
		
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
		sRenderWireframe = sRenderDebugView = false;
		setDebugLevel(DebugLevel::MEDIUM);

		h3dSetOption(H3DOptions::DebugViewMode, sRenderDebugView ? 1.0f : 0.0f);
		h3dSetOption(H3DOptions::WireframeMode, sRenderWireframe ? 1.0f : 0.0f);
	}
	
	void drawText()
	{
		int count = 0;
		for(const std::string& text : sTextList)
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

	void renderFrame(Node activeCamera)
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
		
		h3dRender(activeCamera);
		h3dFinalizeFrame();
		h3dClearOverlays();
		h3dutDumpMessages();
	}
	
    Node createGroupNode(const std::string& name, Node parent)
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
	
	Node getParent(Node node)
	{
		Node parent = h3dGetNodeParent(node);
		return parent;
	}

	bool getNodeChildren(Node node,
						 const std::string& name,
						 NodeList* children,
						 NodeType  childType)
	{
		int childCount = h3dFindNodes(node, name.c_str(), (int)childType);

		if(childCount > 1)
		{
			for(int i = 1; i < childCount; i++)
				children->push_back(h3dGetNodeFindResult(i));
		}

		return childCount > 1 ? true : false;
	}

	bool isTransformed(Node node)
	{
		return h3dCheckNodeTransFlag(node, false);
	}

	void resetTransformFlag(Node node)
	{
		h3dCheckNodeTransFlag(node, true);
	}

	void addText(const std::string& text)
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

	void setNodeFlags(Node node, NodeFlag flag, bool recursive)
	{
		h3dSetNodeFlags(node, (int)flag, recursive);
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

		Resource add(ResourceType type, const std::string& name, int flag)
		{
			Resource res = h3dAddResource((int)type, name.c_str(), flag);
			return res;
		}
		
		Resource get(ResourceType type, const std::string& name)
		{
			Resource res = h3dFindResource((int)type, name.c_str());

			if(res == 0)
			{
				Log::message(name + " is not loaded yet. Loading now..");
				res = add(type, name);
				
				if(!loadAddedResources())
					Log::error("Renderer", name + " not found!");
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
				Log::error("Renderer", "could not set value for uniform " + name);
				return false;
			}

			return true;
			
		}

		Resource createTexture(const std::string& name,
							   int                width,
							   int                height,
							   TextureFormat      format,
							   ResourceFlags      flags)
		{
			Resource texture = h3dCreateTexture(name.c_str(), width, height, (int)format, (int)flags);

			if(texture == 0)
				Log::error("Renderer", "Could not create texture, check renderer logs");

			return texture;
		}
	}

}
