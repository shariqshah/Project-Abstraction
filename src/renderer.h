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

enum class ResourceType : int
{
	MODEL    = H3DResTypes::SceneGraph,
	MATERIAL = H3DResTypes::Material,
	PIPELINE = H3DResTypes::Pipeline,
	GEOMETRY = H3DResTypes::Geometry
};

enum class NodeType : int
{
	GROUP  = H3DNodeTypes::Group,
	MESH   = H3DNodeTypes::Mesh,
	CAMERA = H3DNodeTypes::Camera,
	LIGHT  = H3DNodeTypes::Light,
	MODEL  = H3DNodeTypes::Model
};

enum class Pipeline : uint8_t
{
	FORWARD  = 0,
	DEFERRED = 1,
	HDR      = 2
};

enum class DebugLevel : uint8_t
{
	NONE   = 0,
	MEDIUM = 1,
	HIGH   = 2
};

namespace Renderer
{
	static const Node ROOT_NODE = H3DRootNode;
	
    void initialize(const std::string& path);
	void renderFrame(Node activeCamera);
	void setNodeTransform(Node node,
						  const Vec3 position,
						  const Vec3 rotation,
						  const Vec3 scale);
	void getNodeTransform(Node node,
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

		bool isLoaded(Resource resource);
		bool remove(Resource resource);
		bool loadAddedResources();
		bool setUniform(Resource material, const std::string& name, glm::vec4 value);
	}
}

#endif
