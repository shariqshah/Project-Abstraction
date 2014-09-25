#ifndef RENDERER_H
#define RENDERER_H

#define GLM_FORCE_RADIANS

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/quaternion.hpp"
#include "../include/horde3d/Horde3D.h"
#include "../include/horde3d/Horde3DUtils.h"

#include <iostream>

typedef H3DNode Node;
typedef H3DRes  Resource;

namespace Renderer
{
    void initialize();
	void syncTransform(glm::vec3 position,
					   glm::vec3 rotation,
					   glm::vec3 scale,
					   Node node);
    void setNodeParam(Node node, int param, int value);
    void setNodeParam(Node node, int param, int compID, float value);
    void setCameraView(Node cameraNode,
					   float fov,
					   float aspect,
					   float nearZ,
					   float farZ);
    void resizePipelineBuffers(int width, int height);
    Node createCamera(std::string name, Node parent = H3DRootNode);
    Node createNode(std::string name, Node parent = H3DRootNode);
    Node createGroupNode(std::string name, Node parent = H3DRootNode);
	

    Resource getCurrentPipeline();
}

#endif
