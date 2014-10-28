#ifndef _model_h
#define _model_h

#include "component.h"
#include "renderer.h"

class Model : public Component
{
	Resource    mResourceID;
	Node        mNode;
	std::string mFilename;

public:
	Model(Node parent, const std::string& name);
	Model(const std::string& name);
	~Model();
	
	std::string getFilename();
	Resource    getResourceID();
	Node        getNode();
	float*      getVertices();
	int         getVertexCount();
	
	const   static std::string sName;
	
	virtual const  std::string getName();
};
	
#endif
