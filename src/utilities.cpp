#include "utilities.h"

namespace Utils
{
	namespace
	{
		static std::string textToRender = "";
	}
	
    std::string toString(glm::vec3 vector)
    {
        std::string vectorString;
        vectorString = "(" + std::to_string(vector.x) + ", " +
                             std::to_string(vector.y) + ", " +
                             std::to_string(vector.z) + ")";

        return vectorString;
    }

    // btVector3 glmToBullet(glm::vec3 glmVec3)
    // {
    //     btVector3 bulletVec3(glmVec3.x,
    //                          glmVec3.y,
    //                          glmVec3.z);
    //     return bulletVec3;
    // }

    // btQuaternion glmToBullet(glm::quat glmQuat)
    // {
    //     btQuaternion bulletQuat(glmQuat.x,
    //                             glmQuat.y,
    //                             glmQuat.z,
    //                             glmQuat.w);
    //     return bulletQuat;
    // }

    // glm::vec3 bulletToGlm(btVector3 bulletVec3)
    // {
    //     glm::vec3 glmVec3(bulletVec3.x(),
    //                       bulletVec3.y(),
    //                       bulletVec3.z());
    //     return glmVec3;
    // }

    // glm::quat bulletToGlm(btQuaternion bulletQuat)
    // {
    //     glm::quat glmQuat(bulletQuat.w(),
    //                       bulletQuat.x(),
    //                       bulletQuat.y(),
    //                       bulletQuat.z());
    //     return glmQuat;
    // }

    // std::string toString(btVector3 vector)
    // {
    //     std::string vectorString;
    //     vectorString = "(" + std::to_string(vector.x()) + ", " +
    //                          std::to_string(vector.y()) + ", " +
    //                          std::to_string(vector.z()) + ")";

    //     return vectorString;
    // }

	// btVector3 glmToBullet(const glm::vec3 *glmVec3)
    // {
    //     btVector3 bulletVec3(glmVec3->x,
    //                          glmVec3->y,
    //                          glmVec3->z);
    //     return bulletVec3;
    // }
	
    void initializeScreenText(std::string fontName)
    {
        // TODO: add initalization here 
    }

    void addText(std::string text)
    {
        textToRender += text + "\n";
    }

    std::string toString(glm::vec4 vector)
    {
        std::string vectorString;
        vectorString = "(" + std::to_string(vector.x) + ", " +
                             std::to_string(vector.y) + ", " +
                             std::to_string(vector.z) + ", " +
                             std::to_string(vector.w) + ")";

        return vectorString;
    }

    std::string toString(const glm::vec3 *vector)
    {
        std::string vectorString;
        vectorString = "(" + std::to_string(vector->x) + ", " +
                             std::to_string(vector->y) + ", " +
                             std::to_string(vector->z) + ")";

        return vectorString;
    }

}
