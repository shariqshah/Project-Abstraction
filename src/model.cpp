#include "model.h"

Model::Model(Node parent, std::string name)
{
	mFilename = name;
	mResourceID = Renderer::createModel(mFilename);
	mNode = Renderer::createNode(mResourceID, parent);
	mName = "Model";
	mType = ComponentType::MODEL;
}

Resource Model::getResourceID()
{
	return mResourceID;
}

std::string Model::getFilename()
{
	return mFilename;
}
