#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

#include <string>
#include <vector>
#include <map>

class Animator {
public:

    glm::mat4 globalInvTransf;

    Animator(glm::mat4 globalInvTransf);

    void boneTransform(float timeSecs, const aiScene* mScene, std::map<std::string, aiNodeAnim*> nodeAnimMap, std::vector<Mesh>* meshes);

    void readNodes(float animTime, const aiNode* pNode, glm::mat4 parentTransf, std::map<std::string, aiNodeAnim*> nodeAnimMap, std::vector<Mesh>* meshes);

    unsigned int FindPosition(float animTime, const aiNodeAnim* pNodeAnim);

    unsigned int FindRotation(float animTime, const aiNodeAnim* pNodeAnim);

    unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    glm::mat4 static aiToGlm(aiMatrix4x4 m);

    glm::mat4 static aiToGlm(aiMatrix3x3 m);

};
#endif