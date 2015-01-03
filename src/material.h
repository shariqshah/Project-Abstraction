#ifndef material_H
#define material_H

#include <vector>
#include <iostream>

typedef uint32_t Node;

enum Mat_Type
{
	MAT_UNSHADED = 0,
	MAT_UNSHADED_TEXTURED,
	MAT_PHONG,
	MAT_PHONG_TEXTURED		
};

namespace Material
{
	const static std::vector<Mat_Type> MATERIAL_LIST = { MAT_UNSHADED};
														 // MAT_UNSHADED_TEXTURED,
														 // MAT_PHONG,
														 // MAT_PHONG_TEXTURED };
	
	void              initialize();
	void              cleanup();
	std::vector<int>* getRegisteredModels(Mat_Type material);
    int               getShaderIndex(Mat_Type material);
	bool              registerModel(int modelIndex, Mat_Type material);
	bool              unRegisterModel(int modelIndex, Mat_Type material);
}

#endif
