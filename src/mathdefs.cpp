#include "mathdefs.h"
#include "scriptengine.h"
#include "passert.h"

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

	Vec2& vec2OpAddAssign(const Vec2& other, Vec2& self)
	{
		return self += other;
	}

	Vec2& vec2OpAddAssignFloat(const float val, Vec2& self)
	{
		return self += val;
	}

	Vec2& vec2OpAddAssignInt(const int val, Vec2& self)
	{
		return self += val;
	}

	Vec2& vec2OpMulAssign(const Vec2& other, Vec2& self)
	{
		return self *= other;
	}

	Vec2& vec2OpMulAssignFloat(const float val, Vec2& self)
	{
		return self *= val;
	}

	Vec2& vec2OpMulAssignInt(const int val, Vec2& self)
	{
		return self *= val;
	}

	Vec2& vec2OpDivAssign(const Vec2& other, Vec2& self)
	{
		return self /= other;
	}

	Vec2& vec2OpDivAssignFloat(const float val, Vec2& self)
	{
		return self /= val;
	}

	Vec2& vec2OpDivAssignInt(const int val, Vec2& self)
	{
		return self /= val;
	}

	Vec2& vec2OpSubAssign(const Vec2& other, Vec2& self)
	{
		return self -= other;
	}

	Vec2& vec2OpSubAssignFloat(const float val, Vec2& self)
	{
		return self -= val;
	}

	Vec2& vec2OpSubAssignInt(const int val, Vec2& self)
	{
		return self -= val;
	}

	Vec2 vec2OpAdd(const Vec2& other, const Vec2& self)
	{
		return self + other;
	}

	Vec2 vec2OpSub(const Vec2& other, const Vec2& self)
	{
		return self - other;
	}

	Vec2 vec2OpMul(const Vec2& other, const Vec2& self)
	{
		return self * other;
	}

	Vec2 vec2OpDiv(const Vec2& other, const Vec2& self)
	{
		return self / other;
	}

	bool vec2OpCompare(const Vec2& other, const Vec2& self)
	{
		return other == self;
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

	Vec3& vec3OpAddAssign(const Vec3& other, Vec3& self)
	{
		return self += other;
	}

	Vec3& vec3OpAddAssignFloat(const float val, Vec3& self)
	{
		return self += val;
	}

	Vec3& vec3OpAddAssignInt(const int val, Vec3& self)
	{
		return self += val;
	}

	Vec3& vec3OpMulAssign(const Vec3& other, Vec3& self)
	{
		return self *= other;
	}

	Vec3& vec3OpMulAssignFloat(const float val, Vec3& self)
	{
		return self *= val;
	}

	Vec3& vec3OpMulAssignInt(const int val, Vec3& self)
	{
		return self *= val;
	}

	Vec3& vec3OpDivAssign(const Vec3& other, Vec3& self)
	{
		return self /= other;
	}

	Vec3& vec3OpDivAssignFloat(const float val, Vec3& self)
	{
		return self /= val;
	}

	Vec3& vec3OpDivAssignInt(const int val, Vec3& self)
	{
		return self /= val;
	}

	Vec3& vec3OpSubAssign(const Vec3& other, Vec3& self)
	{
		return self -= other;
	}

	Vec3& vec3OpSubAssignFloat(const float val, Vec3& self)
	{
		return self -= val;
	}

	Vec3& vec3OpSubAssignInt(const int val, Vec3& self)
	{
		return self -= val;
	}

	Vec3 vec3OpAdd(const Vec3& other, const Vec3& self)
	{
		return self + other;
	}

	Vec3 vec3OpSub(const Vec3& other, const Vec3& self)
	{
		return self - other;
	}

	Vec3 vec3OpMul(const Vec3& other, const Vec3& self)
	{
		return self * other;
	}

	Vec3 vec3OpDiv(const Vec3& other, const Vec3& self)
	{
		return self / other;
	}

	bool vec3OpCompare(const Vec3& other, const Vec3& self)
	{
		return other == self;
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

	Vec4& vec4OpAddAssign(const Vec4& other, Vec4& self)
	{
		return self += other;
	}

	Vec4& vec4OpAddAssignFloat(const float val, Vec4& self)
	{
		return self += val;
	}

	Vec4& vec4OpAddAssignInt(const int val, Vec4& self)
	{
		return self += val;
	}

	Vec4& vec4OpMulAssign(const Vec4& other, Vec4& self)
	{
		return self *= other;
	}

	Vec4& vec4OpMulAssignFloat(const float val, Vec4& self)
	{
		return self *= val;
	}

	Vec4& vec4OpMulAssignInt(const int val, Vec4& self)
	{
		return self *= val;
	}

	Vec4& vec4OpDivAssign(const Vec4& other, Vec4& self)
	{
		return self /= other;
	}

	Vec4& vec4OpDivAssignFloat(const float val, Vec4& self)
	{
		return self /= val;
	}

	Vec4& vec4OpDivAssignInt(const int val, Vec4& self)
	{
		return self /= val;
	}

	Vec4& vec4OpSubAssign(const Vec4& other, Vec4& self)
	{
		return self -= other;
	}

	Vec4& vec4OpSubAssignFloat(const float val, Vec4& self)
	{
		return self -= val;
	}

	Vec4& vec4OpSubAssignInt(const int val, Vec4& self)
	{
		return self -= val;
	}

	Vec4 vec4OpAdd(const Vec4& other, const Vec4& self)
	{
		return self + other;
	}

	Vec4 vec4OpSub(const Vec4& other, const Vec4& self)
	{
		return self - other;
	}

	Vec4 vec4OpMul(const Vec4& other, const Vec4& self)
	{
		return self * other;
	}

	Vec4 vec4OpDiv(const Vec4& other, const Vec4& self)
	{
		return self / other;
	}

	bool vec4OpCompare(const Vec4& other, const Vec4& self)
	{
		return other == self;
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

	Quat& quatOpAddAssign(const Quat& other, Quat& self)
	{
		return self += other;
	}

	Quat& quatOpMulAssign(const Quat& other, Quat& self)
	{
		return self *= other;
	}

	Quat& quatOpMulAssignFloat(const float val, Quat& self)
	{
		return self *= val;
	}

	Quat& quatOpMulAssignInt(const int val, Quat& self)
	{
		return self *= val;
	}

	Quat& quatOpDivAssignFloat(const float val, Quat& self)
	{
		return self /= val;
	}

	Quat& quatOpDivAssignInt(const int val, Quat& self)
	{
		return self /= val;
	}

	Quat quatOpAdd(const Quat& other, const Quat& self)
	{
		return self + other;
	}
    
	Quat quatOpMul(const Quat& other, const Quat& self)
	{
		return self * other;
	}

	bool quatOpCompare(const Quat& other, const Quat& self)
	{
		return other == self;
	}

	
	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();

		// Vec2
		int rc = engine->RegisterObjectType("Vec2", sizeof(Vec2), asOBJ_VALUE | asGetTypeTraits<Vec2>());
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec2", "float x", asOFFSET(Vec2, x)); PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec2", "float y", asOFFSET(Vec2, y)); PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT, "void f()",
											 asFUNCTION(vec2Constructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Vec2 &in)",
											 asFUNCTION(vec2CopyConstructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(float, float)",
											 asFUNCTION(vec2ConstructorFromFloats),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(int, int)",
											 asFUNCTION(vec2ConstructorFromInts),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(int)",
											 asFUNCTION(vec2ConstructorFromSingleInt),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_CONSTRUCT,
											 "void f(float)",
											 asFUNCTION(vec2ConstructorFromSingleFloat),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec2",
											 asBEHAVE_DESTRUCT,
											 "void f()",
											 asFUNCTION(vec2Destructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opAssign(const Vec2 &in)",
										  asFUNCTION(vec2OpAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opAddAssign(const Vec2 &in)",
										  asFUNCTION(vec2OpAddAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opSubAssign(const Vec2 &in)",
										  asFUNCTION(vec2OpSubAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opMulAssign(const Vec2 &in)",
										  asFUNCTION(vec2OpMulAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opDivAssign(const Vec2 &in)",
										  asFUNCTION(vec2OpDivAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opAddAssign(const float)",
										  asFUNCTION(vec2OpAddAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opSubAssign(const float)",
										  asFUNCTION(vec2OpSubAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opMulAssign(const float)",
										  asFUNCTION(vec2OpMulAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opDivAssign(const float)",
										  asFUNCTION(vec2OpDivAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opAddAssign(const int)",
										  asFUNCTION(vec2OpAddAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opSubAssign(const int)",
										  asFUNCTION(vec2OpSubAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opMulAssign(const int)",
										  asFUNCTION(vec2OpMulAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2& opDivAssign(const int)",
										  asFUNCTION(vec2OpDivAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2 opAdd(const Vec2 &in)",
										  asFUNCTION(vec2OpAdd),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2 opMul(const Vec2 &in)",
										  asFUNCTION(vec2OpMul),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2 opDiv(const Vec2 &in)",
										  asFUNCTION(vec2OpDiv),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "Vec2 opSub(const Vec2 &in)",
										  asFUNCTION(vec2OpSub),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec2",
										  "bool opEquals(const Vec2 &in)",
										  asFUNCTION(vec2OpCompare),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		
		// Vec3
		rc = engine->RegisterObjectType("Vec3", sizeof(Vec3), asOBJ_VALUE | asGetTypeTraits<Vec3>());
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec3", "float x", asOFFSET(Vec3, x)); PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec3", "float y", asOFFSET(Vec3, y)); PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec3", "float z", asOFFSET(Vec3, z)); PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT, "void f()",
											 asFUNCTION(vec3Constructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Vec3 &in)",
											 asFUNCTION(vec3CopyConstructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(float, float, float)",
											 asFUNCTION(vec3ConstructorFromFloats),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(int, int, int)",
											 asFUNCTION(vec3ConstructorFromInts),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(int)",
											 asFUNCTION(vec3ConstructorFromSingleInt),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_CONSTRUCT,
											 "void f(float)",
											 asFUNCTION(vec3ConstructorFromSingleFloat),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec3",
											 asBEHAVE_DESTRUCT,
											 "void f()",
											 asFUNCTION(vec3Destructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opAssign(const Vec3 &in)",
										  asFUNCTION(vec3OpAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opAddAssign(const Vec3 &in)",
										  asFUNCTION(vec3OpAddAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opSubAssign(const Vec3 &in)",
										  asFUNCTION(vec3OpSubAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opMulAssign(const Vec3 &in)",
										  asFUNCTION(vec3OpMulAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opDivAssign(const Vec3 &in)",
										  asFUNCTION(vec3OpDivAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opAddAssign(const float)",
										  asFUNCTION(vec3OpAddAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opSubAssign(const float)",
										  asFUNCTION(vec3OpSubAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opMulAssign(const float)",
										  asFUNCTION(vec3OpMulAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opDivAssign(const float)",
										  asFUNCTION(vec3OpDivAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opAddAssign(const int)",
										  asFUNCTION(vec3OpAddAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opSubAssign(const int)",
										  asFUNCTION(vec3OpSubAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opMulAssign(const int)",
										  asFUNCTION(vec3OpMulAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3& opDivAssign(const int)",
										  asFUNCTION(vec3OpDivAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3 opAdd(const Vec3 &in)",
										  asFUNCTION(vec3OpAdd),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3 opMul(const Vec3 &in)",
										  asFUNCTION(vec3OpMul),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3 opDiv(const Vec3 &in)",
										  asFUNCTION(vec3OpDiv),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "Vec3 opSub(const Vec3 &in)",
										  asFUNCTION(vec3OpSub),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec3",
										  "bool opEquals(const Vec3 &in)",
										  asFUNCTION(vec3OpCompare),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );

		// Vec4
		engine->RegisterObjectType("Vec4", sizeof(Vec4), asOBJ_VALUE | asGetTypeTraits<Vec4>());
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec4", "float x", asOFFSET(Vec4, x));  PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec4", "float y", asOFFSET(Vec4, y));  PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec4", "float z", asOFFSET(Vec4, z));  PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Vec4", "float w", asOFFSET(Vec4, w));  PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT, "void f()",
											 asFUNCTION(vec4Constructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Vec4 &in)",
											 asFUNCTION(vec4CopyConstructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Vec3 &in)",
											 asFUNCTION(vec4CopyConstructorFromVec3),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(float, float, float, float)",
											 asFUNCTION(vec4ConstructorFromFloats),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(int, int, int, int)",
											 asFUNCTION(vec4ConstructorFromInts),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(int)",
											 asFUNCTION(vec4ConstructorFromSingleInt),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_CONSTRUCT,
											 "void f(float)",
											 asFUNCTION(vec4ConstructorFromSingleFloat),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Vec4",
											 asBEHAVE_DESTRUCT,
											 "void f()",
											 asFUNCTION(vec4Destructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opAssign(const Vec4 &in)",
										  asFUNCTION(vec4OpAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opAddAssign(const Vec4 &in)",
										  asFUNCTION(vec4OpAddAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opSubAssign(const Vec4 &in)",
										  asFUNCTION(vec4OpSubAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opMulAssign(const Vec4 &in)",
										  asFUNCTION(vec4OpMulAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opDivAssign(const Vec4 &in)",
										  asFUNCTION(vec4OpDivAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opAddAssign(const float)",
										  asFUNCTION(vec4OpAddAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opSubAssign(const float)",
										  asFUNCTION(vec4OpSubAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opMulAssign(const float)",
										  asFUNCTION(vec4OpMulAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opDivAssign(const float)",
										  asFUNCTION(vec4OpDivAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opAddAssign(const int)",
										  asFUNCTION(vec4OpAddAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opSubAssign(const int)",
										  asFUNCTION(vec4OpSubAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opMulAssign(const int)",
										  asFUNCTION(vec4OpMulAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4& opDivAssign(const int)",
										  asFUNCTION(vec4OpDivAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4 opAdd(const Vec4 &in)",
										  asFUNCTION(vec4OpAdd),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4 opMul(const Vec4 &in)",
										  asFUNCTION(vec4OpMul),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4 opDiv(const Vec4 &in)",
										  asFUNCTION(vec4OpDiv),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "Vec4 opSub(const Vec4 &in)",
										  asFUNCTION(vec4OpSub),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Vec4",
										  "bool opEquals(const Vec4 &in)",
										  asFUNCTION(vec4OpCompare),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );

		// Quat
		// engine->RegisterObjectType("Quat", sizeof(Quat), asOBJ_VALUE | asGetTypeTraits<Quat>());
		engine->RegisterObjectType("Quat", sizeof(Quat), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Quat", "float x", asOFFSET(Quat, x));  PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Quat", "float y", asOFFSET(Quat, y));  PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Quat", "float z", asOFFSET(Quat, z));  PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectProperty("Quat", "float w", asOFFSET(Quat, w));  PA_ASSERT(rc >= 0);
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_CONSTRUCT, "void f()",
											 asFUNCTION(quatConstructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_CONSTRUCT,
											 "void f(const Quat &in)",
											 asFUNCTION(quatCopyConstructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_CONSTRUCT,
											 "void f(float, float, float, float)",
											 asFUNCTION(quatConstructorFromFloats),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_CONSTRUCT,
											 "void f(int, int, int, int)",
											 asFUNCTION(quatConstructorFromInts),
											 asCALL_CDECL_OBJFIRST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectBehaviour("Quat",
											 asBEHAVE_DESTRUCT,
											 "void f()",
											 asFUNCTION(quatDestructor),
											 asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat& opAssign(const Quat &in)",
										  asFUNCTION(quatOpAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat& opAddAssign(const Quat &in)",
										  asFUNCTION(quatOpAddAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat& opMulAssign(const Quat &in)",
										  asFUNCTION(quatOpMulAssign),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat& opMulAssign(const float)",
										  asFUNCTION(quatOpMulAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat& opDivAssign(const float)",
										  asFUNCTION(quatOpDivAssignFloat),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat& opMulAssign(const int)",
										  asFUNCTION(quatOpMulAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat& opDivAssign(const int)",
										  asFUNCTION(quatOpDivAssignInt),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat opAdd(const Quat &in)",
										  asFUNCTION(quatOpAdd),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "Quat opMul(const Quat &in)",
										  asFUNCTION(quatOpMul),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
		rc = engine->RegisterObjectMethod("Quat",
										  "bool opEquals(const Quat &in)",
										  asFUNCTION(quatOpCompare),
										  asCALL_CDECL_OBJLAST);
		PA_ASSERT( rc >= 0 );
	}
}
