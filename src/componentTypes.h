#ifndef COMPONENTTYPES_H
#define COMPONENTTYPES_H

enum class ComponentType : int
{
    EMPTY          = -1,
	TRANSFORM      =  1,
	CAMERA         =  2,
	MODEL          =  3,
	LIGHT          =  4,
	RIGIDBODY      =  5,
	NUM_COMPONENTS =  6
};

#endif // COMPONENTTYPES_H
