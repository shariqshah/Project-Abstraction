#ifndef RENDERER_H
#define RENDERER_H

#define GLM_FORCE_RADIANS
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/quaternion.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

#include "../include/horde3d/Horde3D.h"
#include "../include/horde3d/Horde3DUtils.h"

#include "log.h"
#include "settings.h"

#include <iostream>
#include <vector>
#include <algorithm>

typedef H3DNode Node;
typedef H3DRes  Resource;
typedef std::vector<Node> NodeList;

typedef glm::vec3 Vec3;
typedef glm::vec2 Vec2;

enum class NodeFlag : int
{
	INACTIVE     = H3DNodeFlags::Inactive,
	NO_DRAW      = H3DNodeFlags::NoDraw,
	NO_SHADOW    = H3DNodeFlags::NoCastShadow,
	NO_RAY_QUERY = H3DNodeFlags::NoRayQuery,
};

enum class ResourceType : int
{
	MODEL    = H3DResTypes::SceneGraph,
	MATERIAL = H3DResTypes::Material,
	PIPELINE = H3DResTypes::Pipeline,
	GEOMETRY = H3DResTypes::Geometry
};

enum class ResourceFlags : int
{
	NO_QUERY            = H3DResFlags::NoQuery,
	NO_TEX_COMPRESSION  = H3DResFlags::NoTexCompression,
	NO_TEX_MIPMAP       = H3DResFlags::NoTexMipmaps,
	TEX_CUBEMAP_        = H3DResFlags::TexCubemap,
	TEX_DYNAMIC_        = H3DResFlags::TexDynamic,
	TEX_RENDERABLE      = H3DResFlags::TexRenderable,
	TEX_SRGB            = H3DResFlags::TexSRGB
};

enum class NodeType : int
{
	GROUP  = H3DNodeTypes::Group,
	MESH   = H3DNodeTypes::Mesh,
	CAMERA = H3DNodeTypes::Camera,
	LIGHT  = H3DNodeTypes::Light,
	MODEL  = H3DNodeTypes::Model
};

enum Pipeline : uint8_t
{
	FORWARD  = 0,
	DEFERRED = 1,
	HDR      = 2,
	RTT      = 3
};

enum class DebugLevel : uint8_t
{
	NONE   = 0,
	MEDIUM = 1,
	HIGH   = 2
};

enum class TextureFormat : int
{
	UNKNOWN   = H3DFormats::Unknown,
	BGRA8     = H3DFormats::TEX_BGRA8,
	DXT1      = H3DFormats::TEX_DXT1,
	DXT3      = H3DFormats::TEX_DXT3,
	DXT5      = H3DFormats::TEX_DXT5,
	RGBA16F   = H3DFormats::TEX_RGBA16F,
	RGBA32F   = H3DFormats::TEX_RGBA32F
};

namespace Renderer
{
	static const Node ROOT_NODE = H3DRootNode;
	
    void initialize(const std::string& path);
	void renderFrame(Node activeCamera);
	void setNodeTransform(Node       node,
						  const Vec3 position,
						  const Vec3 rotation,
						  const Vec3 scale);
	void getNodeTransform(Node  node,
						  Vec3* position,
						  Vec3* rotation,
						  Vec3* scale);
	void setNodeTransform(Node node, glm::mat4 transformMat);
    void setNodeParam(Node node, int param, int value);
    void setNodeParam(Node node, int param, int compID, float value);
	void setNodeName(Node node, const std::string& name);
    void resizePipelineBuffers(int width, int height);
	void setCurrentCamera(Node cameraNode);
	void addText(const std::string& text);
	void removeCamera(Node camera);
	void setDebugLevel(DebugLevel level);
	void toggleDebugView();
	void toggleWireframe();
	void resetTransformFlag(Node node);
	void setNodeFlags(Node node, NodeFlag flag, bool recursive = true);
	
	bool removeNode(Node node);
	bool setParent(Node child, Node parent);
	bool getNodeChildren(Node node,
						 const std::string& name,
						 NodeList* children,
						 NodeType  childType = NodeType::GROUP);
	bool isTransformed(Node node);

	Node getParent(Node node);
	Node getCurrentCameraNode();
    Node createCamera(const std::string& name, Node parent = ROOT_NODE);
    Node createNode(Resource resource, Node parent = ROOT_NODE);
    Node createGroupNode(const std::string& name, Node parent = ROOT_NODE);

	namespace Resources
	{
		Resource add(ResourceType type, const std::string& name, int flag = 0);
		Resource get(ResourceType type, const std::string& name);
		Resource createTexture(const std::string& name,
							   int                width,
							   int                height,
							   TextureFormat      format,
							   ResourceFlags      flags);

		bool isLoaded(Resource resource);
		bool remove(Resource resource);
		bool loadAddedResources();
		bool setUniform(Resource material, const std::string& name, glm::vec4 value);
	}
}

#endif
