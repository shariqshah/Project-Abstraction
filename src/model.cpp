#include "model.h"

Model::Model(Node parent, std::string name)
{
	mName = "Model";
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

Resource Model::getResourceID()
{
	return mResourceID;
}

std::string Model::getFilename()
{
	return mFilename;
}
