#ifndef COMPONENTTYPES_H
#define COMPONENTTYPES_H

enum class ComponentType : long
{
    NO_COMPONENT = 0,
	TRANSFORM    = 1 << 0,
	CAMERA       = 1 << 1,
	MODEL        = 1 << 2,
	LIGHT        = 1 << 3,
	RIGIDBODY    = 1 << 4
};

#endif // COMPONENTTYPES_H
