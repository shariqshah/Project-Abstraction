#ifndef COMPONENTTYPES_H
#define COMPONENTTYPES_H

enum class ComponentType : long
{
    NO_COMPONENT  = 0,
	TRANSFORM     = 1 << 0,
	CAMERA        = 1 << 1
};

#endif // COMPONENTTYPES_H
