#ifndef PHYSICS_H
#define PHYSICS_H

#include "physicsdebugdrawer.h"
#include "mathdefs.h"
#include "datatypes.h"

class  btDiscreteDynamicsWorld;
class  btMotionState;
class  CollisionShape;
struct CCamera;
struct CTransform;
struct CModel;
struct GameObject;

struct CollisionData
{
	Node collidingObjNode;
	Vec3 worldPosA;
	Vec3 worldPosB;
	Vec3 normal;
};

namespace Physics
{
	enum class DBG_Mode : int
	{
		NONE             = btIDebugDraw::DBG_NoDebug,
		WIREFRAME        = btIDebugDraw::DBG_DrawWireframe,
		AABB             = btIDebugDraw::DBG_DrawAabb,
		CONTACTS         = btIDebugDraw::DBG_DrawContactPoints,
		FEATURES_TEXT    = btIDebugDraw::DBG_DrawFeaturesText,
		NO_DEACTIVATION  = btIDebugDraw::DBG_NoDeactivation,
		NO_HELP_TEXT     = btIDebugDraw::DBG_NoHelpText,
		TEXT             = btIDebugDraw::DBG_DrawText,
		PROFIEL_TIMING   = btIDebugDraw::DBG_ProfileTimings,
		ENABLE_SAT_COMP  = btIDebugDraw::DBG_EnableSatComparison,
		DISABLE_LCP      = btIDebugDraw::DBG_DisableBulletLCP,
		ENABLE_CCD       = btIDebugDraw::DBG_EnableCCD,
		CONSTRAINTS      = btIDebugDraw::DBG_DrawConstraints,
		CONSTRAINT_LIMIT = btIDebugDraw::DBG_DrawConstraintLimits,
        NORMALS          = btIDebugDraw::DBG_DrawNormals,
		MAX              = btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE
	};
	void initialize(Vec3 gravity);
	void update(float deltaTime);
    void draw();
	void cleanup();
	void setGravity(Vec3 gravity);
    void enableDebugDraw(bool enable);
	void nextDebugMode();
	void setDebugMode(DBG_Mode debugMode);
	void addCollisionShape(CollisionShape* shape);
	void generateBindings();
	bool isEnabled();
	bool isDebugDrawerEnabled();
	void enable(bool enable);
	void enableDebugDraw(bool enable);
	const Vec3 getGravity();
	btDiscreteDynamicsWorld* getWorld();
}

#endif
