#include <GL/gl.h>

#include "renderer.h"
#include "texture.h"
#include "shader.h"
#include "material.h"
#include "model.h"
#include "camera.h"

namespace Renderer
{
	namespace
	{
        char*       contentDir;
		std::vector<std::string> sTextList;
		Vec2        sFontPos;
		Resource    sFontMat;
		Resource    sPanelMat;
		float       sFontSize;
		Resource    sLightMat;
		DebugLevel  sDebugLevel;
		bool        sRenderWireframe;
		bool        sRenderDebugView;
		const char* texDir         = "/textures/";
		const char* shaderDir      = "/shaders/";
		const char* contentDirName = "/../content";
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

	void setClearColor(const Vec3 clearColor)
	{
		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0);
	}

	void initialize(const char* path)
	{
		contentDir = (char*)malloc(sizeof(char) * (strlen(path) + strlen(contentDirName)) + 1);
		assert(contentDir != NULL);
		
        strcpy(contentDir, path);
		strcat(contentDir, contentDirName);

		glClearColor(0.55, 0.6, 0.8, 1.0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDepthFunc(GL_LEQUAL);

        char* texturePath = (char *)malloc(sizeof(char) *
										   (strlen(contentDir) + strlen(texDir)) + 1);
        strcpy(texturePath, contentDir);
		strcat(texturePath, texDir);
		
		char* shaderPath = (char *)malloc(sizeof(char) *
										   (strlen(contentDir) + strlen(shaderDir)) + 1);
        strcpy(shaderPath, contentDir);
		strcat(shaderPath, shaderDir);

		Texture::initialize(texturePath);
		Shader::initialize(shaderPath);
		Material::initialize();
		
		free(texturePath);
		free(shaderPath);
	}

	void cleanup()
	{
		free(contentDir);
		Texture::cleanup();
		Shader::cleanup();
		Model::cleanup();
		Material::cleanup();
		Model::cleanup();
		Camera::cleanup();
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

	void renderFrame(CCamera* camera)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Model::renderAllModels(camera);
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
            // return h3dutLoadResourcesFromDisk(contentDir.c_str());
			return true;
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
