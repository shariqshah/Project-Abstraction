#ifndef _model_h
#define _model_h

#include "component.h"
#include "renderer.h"

class Model : public Component
{
	Resource mResourceID;
	Node mNode;
	std::string mFilename;

public:
	Model(Node parent, std::string name);
	~Model();
	
	std::string getFilename();
	Resource getResourceID();
	Node getNode();
};
	
#endif
