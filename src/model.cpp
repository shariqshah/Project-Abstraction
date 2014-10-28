#include "model.h"

const std::string Model::sName = "Model";

Model::Model(Node parent, const std::string& name)
{
	mType = ComponentType::MODEL;
	mFilename = name;
	mResourceID = Renderer::Resources::get(ResourceType::MODEL, mFilename);

	if(mResourceID == 0)
		mValid = false;
	else
	{
		mNode = Renderer::createNode(mResourceID, parent);

		if(mNode == 0)
			mValid = false;
	}
}

Model::Model(const std::string& name)
{
	// If model is created without a parent node then it cannot be added to
	// gameobjects. It can only be used to provide collision mesh
	mType = ComponentType::MODEL;
	mFilename = name;
	mResourceID = Renderer::Resources::get(ResourceType::MODEL, mFilename);
	mValid = false;
	mNode = Renderer::createNode(mResourceID, Renderer::ROOT_NODE);
}


Model::~Model()
{
	if(mResourceID != 0 && Renderer::Resources::isLoaded(mResourceID))
	{
		if(!Renderer::Resources::remove(mResourceID))
			Log::warning("Resource not removed for Model Component!");

		if(mNode != 0)
		{
			if(!Renderer::removeNode(mNode))
				Log::warning("Node not removed for Model Component");
		}
	}
}

Resource Model::getResourceID()
{
	return mResourceID;
}

std::string Model::getFilename()
{
	return mFilename;
}

float* Model::getVertices()
{
	return Renderer::Model::getVertices(mNode);
}

int Model::getVertexCount()
{
	return Renderer::Model::getVertexCount(mNode);
}

const std::string Model::getName()
{
	return sName;
}
