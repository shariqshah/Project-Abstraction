#include "utilities.h"
#include "../include/bullet/btBulletDynamicsCommon.h"

namespace Utils
{	
    const std::string toString(const Vec3 vector)
    {
        std::string vectorString;
        vectorString = "(" + std::to_string(vector.x) + ", " +
                             std::to_string(vector.y) + ", " +
                             std::to_string(vector.z) + ")";

        return vectorString;
    }

	const std::string toString(const Quat quat)
	{
		std::string quatString;
        quatString = "(" + std::to_string(quat.x) + ", " +
                           std::to_string(quat.y) + ", " +
                           std::to_string(quat.z) + ", " +
                           std::to_string(quat.w) + ")";

        return quatString;
	}

    btVector3 toBullet(const Vec3 glmVec3)
    {
        btVector3 bulletVec3(btScalar(glmVec3.x),
                             btScalar(glmVec3.y),
                             btScalar(glmVec3.z));
        return bulletVec3;
    }

    btQuaternion toBullet(const Quat glmQuat)
    {
        btQuaternion bulletQuat(btScalar(glmQuat.x),
                                btScalar(glmQuat.y),
                                btScalar(glmQuat.z),
                                btScalar(glmQuat.w));
        return bulletQuat;
    }

    Vec3 toGlm(const btVector3 bulletVec3)
    {
        Vec3 glmVec3(bulletVec3.x(),
                          bulletVec3.y(),
                          bulletVec3.z());
        return glmVec3;
    }

    Quat toGlm(const btQuaternion bulletQuat)
    {
        Quat glmQuat(bulletQuat.w(),
                          bulletQuat.x(),
                          bulletQuat.y(),
                          bulletQuat.z());
        return glmQuat;
    }

    const std::string toString(const btVector3 vector)
    {
        std::string vectorString;
        vectorString = "(" + std::to_string(vector.x()) + ", " +
                             std::to_string(vector.y()) + ", " +
                             std::to_string(vector.z()) + ")";

        return vectorString;
    }

	btVector3 toBullet(const Vec3 *glmVec3)
    {
        btVector3 bulletVec3(btScalar(glmVec3->x),
                             btScalar(glmVec3->y),
                             btScalar(glmVec3->z));
        return bulletVec3;
    }
    
    const std::string toString(const Vec4 vector)
    {
        std::string vectorString;
        vectorString = "(" + std::to_string(vector.x) + ", " +
                             std::to_string(vector.y) + ", " +
                             std::to_string(vector.z) + ", " +
                             std::to_string(vector.w) + ")";

        return vectorString;
    }

    const std::string toString(const Vec3 *vector)
    {
        std::string vectorString;
        vectorString = "(" + std::to_string(vector->x) + ", " +
                             std::to_string(vector->y) + ", " +
                             std::to_string(vector->z) + ")";

        return vectorString;
    }
}
