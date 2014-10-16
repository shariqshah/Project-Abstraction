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

enum class ResourceType : int
{
	MODEL    = H3DResTypes::SceneGraph,
	MATERIAL = H3DResTypes::Material,
	PIPELINE = H3DResTypes::Pipeline
};

enum class Pipeline : uint8_t
{
	FORWARD = 0,
	DEFERRED = 1,
	HDR = 2
};

enum class DebugLevel : uint8_t
{
	NONE   = 0,
	MEDIUM = 1,
	HIGH   = 2
};

namespace Renderer
{
	static const Node Root = H3DRootNode;
	
    void initialize();
	void renderFrame();
	void setNodeTransform(Node node,
						  const glm::vec3 position,
						  const glm::vec3 rotation,
						  const glm::vec3 scale);
	void getNodeTransform(Node node,
						  glm::vec3* position,
						  glm::vec3* rotation,
						  glm::vec3* scale);
	void setNodeTransform(Node node, glm::mat4 transformMat);
    void setNodeParam(Node node, int param, int value);
    void setNodeParam(Node node, int param, int compID, float value);
	void setNodeName(Node node, const std::string& name);
	void drawText();
    void resizePipelineBuffers(int width, int height);
	void setCurrentCamera(Node cameraNode);
	void addText(const std::string& text);
	void removeCamera(Node camera);
	void setDebugLevel(DebugLevel level);
	void toggleDebugView();
	void toggleWireframe();
	
	bool removeNode(Node node);
	bool setParent(Node child, Node parent);
	bool getNodeChildren(Node node, const std::string& name, NodeList* children);

	Node getParent(Node node);
	Node getCurrentCameraNode();
    Node createCamera(const std::string& name, Node parent = Root);
    Node createNode(Resource resource, Node parent = Root);
    Node createGroupNode(const std::string& name, Node parent = Root);
	
	namespace Camera
	{
		void setViewportSize(Node camera, int width, int height);
		void setViewportPos(Node camera, int x, int y);
		void setView(Node  cameraNode,
					 float fov,
					 float aspect,
					 float nearZ,
					 float farZ);
		void setPipeline(Node camera, Pipeline pipeline);
		void setOcclusionCulling(Node camera, bool enable);
		void setOrthgraphic(Node camera, bool enable);
	}

	namespace Resources
	{
		Resource add(ResourceType type, const std::string& name, int flag = 0);
		Resource get(ResourceType type, const std::string& name);

		bool isLoaded(Resource resource);
		bool remove(Resource resource);
		bool loadAddedResources();
		bool setUniform(Resource material, const std::string& name, glm::vec4 value);
	}

	namespace Light
	{
		Node create(Node parent,
					const std::string& name,
					Resource material = 0,
					const std::string& lightContext  = "LIGHTING",
					const std::string& shadowContext = "SHADOWMAP");
		
		void setRadius(Node light, float radius);
		void setColor(Node light, glm::vec3 color);
		void setFov(Node light, float fov);
		void setShadowMapCount(Node light, int count);
		void setShadowMapBias(Node light, float bias);
		void setIntensity(Node light, float intensity);
		void setShadowSplitLambda(Node light, float splitLambda);
	}
}

#endif
