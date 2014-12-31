#ifndef material_H
#define material_H

enum Mat_Type
{
	MAT_UNSHADED = 0,
	MAT_UNSHADED_TEXTURED,
	MAT_PHONG,
	MAT_PHONG_TEXTURED		
};

namespace Material
{
	void initialize();
}

#endif
