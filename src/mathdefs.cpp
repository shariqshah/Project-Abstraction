#include "mathdefs.h"
#include "scriptengine.h"

namespace MathTypes
{
	// VEC2
	void vec2Constructor(void *memory)
	{
		new(memory) Vec2();
	}

	void vec2CopyConstructor(const Vec2& other, void *memory)
	{
		new(memory) Vec2(other);
	}
	
	void vec2ConstructorFromFloats(void *memory, float x, float y)
	{
		new(memory) Vec2(x, y);
	}

	void vec2ConstructorFromSingleFloat(void *memory, float val)
	{
		new(memory) Vec2(val);
	}

	void vec2ConstructorFromSingleInt(void *memory, int val)
	{
		new(memory) Vec2(val);
	}

	void vec2ConstructorFromInts(void *memory, int x, int y)
	{
		new(memory) Vec2(x, y);
	}

	void vec2Destructor(void *memory)
	{
		((Vec2*)memory)->~Vec2();
	}

	Vec2& vec2OpAssign(const Vec2& other, Vec2& self)
	{
		self = other;
		return self;
	}
	
	// VEC3
	void vec3Constructor(void *memory)
	{
		new(memory) Vec3();
	}

	void vec3CopyConstructor(const Vec3& other, void *memory)
	{
		new(memory) Vec3(other);
	}
	
	void vec3ConstructorFromFloats(void *memory, float x, float y, float z)
	{
		new(memory) Vec3(x, y, z);
	}

	void vec3ConstructorFromSingleFloat(void *memory, float val)
	{
		new(memory) Vec3(val);
	}

	void vec3ConstructorFromSingleInt(void *memory, int val)
	{
		new(memory) Vec3(val);
	}

	void vec3ConstructorFromInts(void *memory, int x, int y, int z)
	{
		new(memory) Vec3(x, y, z);
	}

	void vec3Destructor(void *memory)
	{
		((Vec3*)memory)->~Vec3();
	}

	Vec3& vec3OpAssign(const Vec3& other, Vec3& self)
	{
		self = other;
		return self;
	}

	// VEC4
	void vec4Constructor(void *memory)
	{
		new(memory) Vec4();
	}

	void vec4CopyConstructor(const Vec4& other, void *memory)
	{
		new(memory) Vec4(other);
	}

	void vec4CopyConstructorFromVec3(const Vec3& other, void *memory)
	{
		new(memory) Vec4(other.x, other.y, other.z, 0);
	}
	
	void vec4ConstructorFromFloats(void *memory, float x, float y, float z, float w)
	{
		new(memory) Vec4(x, y, z, w);
	}

	void vec4ConstructorFromSingleFloat(void *memory, float val)
	{
		new(memory) Vec4(val);
	}

	void vec4ConstructorFromSingleInt(void *memory, int val)
	{
		new(memory) Vec4(val);
	}

	void vec4ConstructorFromInts(void *memory, int x, int y, int z, int w)
	{
		new(memory) Vec4(x, y, z, w);
	}

	Vec4& vec4OpAssign(const Vec4& other, Vec4& self)
	{
		self = other;
		return self;
	}

	void vec4Destructor(void *memory)
	{
		((Vec4*)memory)->~Vec4();
	}

	// QUAT
	void quatConstructor(void *memory)
	{
		new(memory) Quat();
	}

	void quatCopyConstructor(const Quat& other, void *memory)
	{
		new(memory) Quat(other);
	}
	
	void quatConstructorFromFloats(void *memory, float x, float y, float z, float w)
	{
		new(memory) Quat(x, y, z, w);
	}

	void quatConstructorFromInts(void *memory, int x, int y, int z, int w)
	{
		new(memory) Quat(x, y, z, w);
	}

	void quatDestructor(void *memory)
	{
		((Quat*)memory)->~Quat();
	}

	Quat& quatOpAssign(const Quat& other, Quat& self)
	{
		self = other;
		return self;
	}

	
	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();

		// Vec2
		int rc = engine->RegisterObjectType("Vec2", sizeof(Vec2), asOBJ_VALUE | asGetTypeTraits<Vec2>());
		assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec2", "float x", asOFFSET(Vec2, x)); assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec2", "float y", asOFFSET(Vec2, y)); assert(rc >= 0);
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT, "void f()",
											 asFUNCTION(vec2Constructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Vec2 &in)",
											 asFUNCTION(vec2CopyConstructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(float, float)",
											 asFUNCTION(vec2ConstructorFromFloats),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(int, int)",
											 asFUNCTION(vec2ConstructorFromInts),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(int)",
											 asFUNCTION(vec2ConstructorFromSingleInt),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(float)",
											 asFUNCTION(vec2ConstructorFromSingleFloat),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_DESTRUCT,
											 "void f()",
											 asFUNCTION(vec2Destructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opAssign(const Vec2 &in)",
										  asFUNCTION(vec2OpAssign),
										  asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		
		// Vec3
		rc = engine->RegisterObjectType("Vec3", sizeof(Vec3), asOBJ_VALUE | asGetTypeTraits<Vec3>());
		assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec3", "float x", asOFFSET(Vec3, x)); assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec3", "float y", asOFFSET(Vec3, y)); assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec3", "float z", asOFFSET(Vec3, z)); assert(rc >= 0);
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT, "void f()",
											 asFUNCTION(vec3Constructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Vec3 &in)",
											 asFUNCTION(vec3CopyConstructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(float, float, float)",
											 asFUNCTION(vec3ConstructorFromFloats),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(int, int, int)",
											 asFUNCTION(vec3ConstructorFromInts),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(int)",
											 asFUNCTION(vec3ConstructorFromSingleInt),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(float)",
											 asFUNCTION(vec3ConstructorFromSingleFloat),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_DESTRUCT,
											 "void f()",
											 asFUNCTION(vec3Destructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opAssign(const Vec3 &in)",
										  asFUNCTION(vec3OpAssign),
										  asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );

		// Vec4
		engine->RegisterObjectType("Vec4", sizeof(Vec4), asOBJ_VALUE | asGetTypeTraits<Vec4>());
		assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec4", "float x", asOFFSET(Vec4, x));  assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec4", "float y", asOFFSET(Vec4, y));  assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec4", "float z", asOFFSET(Vec4, z));  assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec4", "float w", asOFFSET(Vec4, w));  assert(rc >= 0);
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT, "void f()",
											 asFUNCTION(vec4Constructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Vec4 &in)",
											 asFUNCTION(vec4CopyConstructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Vec3 &in)",
											 asFUNCTION(vec4CopyConstructorFromVec3),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(float, float, float, float)",
											 asFUNCTION(vec4ConstructorFromFloats),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(int, int, int, int)",
											 asFUNCTION(vec4ConstructorFromInts),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(int)",
											 asFUNCTION(vec4ConstructorFromSingleInt),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(float)",
											 asFUNCTION(vec4ConstructorFromSingleFloat),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_DESTRUCT,
											 "void f()",
											 asFUNCTION(vec4Destructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opAssign(const Vec4 &in)",
										  asFUNCTION(vec4OpAssign),
										  asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );

		// Quat
		engine->RegisterObjectType("Quat", sizeof(Quat), asOBJ_VALUE | asGetTypeTraits<Quat>());
		assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Quat", "float x", asOFFSET(Quat, x));  assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Quat", "float y", asOFFSET(Quat, y));  assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Quat", "float z", asOFFSET(Quat, z));  assert(rc >= 0);
		rc = engine->RegisterObjectProperty("Quat", "float w", asOFFSET(Quat, w));  assert(rc >= 0);
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_CONSTRUCT, "void f()",
											 asFUNCTION(quatConstructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Quat &in)",
											 asFUNCTION(quatCopyConstructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_CONSTRUCT,
											 "void f(float, float, float, float)",
											 asFUNCTION(quatConstructorFromFloats),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_CONSTRUCT,
											 "void f(int, int, int, int)",
											 asFUNCTION(quatConstructorFromInts),
											 asCALL_CDECL_OBJFIRST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_DESTRUCT,
											 "void f()",
											 asFUNCTION(quatDestructor),
											 asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat& opAssign(const Quat &in)",
										  asFUNCTION(quatOpAssign),
										  asCALL_CDECL_OBJLAST);
		assert( rc >= 0 );
	}
}
