#include "Animator.h"



    Animator::Animator(glm::mat4 globalInvTransf)
    {
        this->globalInvTransf = globalInvTransf;
    }

    void Animator::boneTransform(float timeSecs, const aiScene * mScene, map<string, aiNodeAnim*> nodeAnimMap, vector<Mesh> *meshes)
    {
        glm::mat4 ident = glm::mat4(1.0);
        float TicksPerSecond = (float)(mScene->mAnimations[0]->mTicksPerSecond != 0 ? mScene->mAnimations[0]->mTicksPerSecond : 25.0f);
        float TimeInTicks = timeSecs * TicksPerSecond;
        float AnimationTime = fmod(TimeInTicks, (float)mScene->mAnimations[0]->mDuration);

        readNodes(AnimationTime, mScene->mRootNode, ident, nodeAnimMap, meshes);
    }

	void Animator::readNodes(float animTime, const aiNode* pNode, glm::mat4 parentTransf, map<string, aiNodeAnim*> nodeAnimMap, vector<Mesh> *meshes)
	{
		glm::mat4 nodeTransf = aiToGlm(pNode->mTransformation);
		aiNodeAnim* pNodeAnim = nodeAnimMap[pNode->mName.C_Str()];
		if (pNodeAnim)
		{
			aiVector3D Scaling;
            CalcInterpolatedScaling(Scaling, animTime, pNodeAnim);
            glm::mat4 ScalingM = glm::mat4(1.0f);
            ScalingM = glm::transpose(glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z)));
        
            // Interpolate rotation and generate rotation transformation matrix
            aiQuaternion RotationQ;
            CalcInterpolatedRotation(RotationQ, animTime, pNodeAnim); 
            glm::mat4 RotationM = aiToGlm(RotationQ.GetMatrix());

            // Interpolate translation and generate translation transformation matrix
            aiVector3D Translation;
            CalcInterpolatedPosition(Translation, animTime, pNodeAnim);
            glm::mat4 TranslationM = glm::mat4(1.0f);;
            TranslationM = glm::transpose(glm::translate(TranslationM, glm::vec3(Translation.x, Translation.y, Translation.z)));
        
            // Combine the above transformations
            nodeTransf = ScalingM * RotationM * TranslationM;
		}

        glm::mat4 globalTransf = nodeTransf * parentTransf;

        //cout << meshes->size() << endl;
        for (unsigned int i = 0; i < meshes->size(); i++)
        {
            for (unsigned int j = 0; j < (*meshes)[i].bones.size(); j++)
            {
                if ((*meshes)[i].bones[j].name == pNode->mName)
                {
                    //cout << "IM HERE" << endl;
                    (*meshes)[i].bones[j].finalTransf =  (*meshes)[i].bones[j].offsetMat * globalTransf * globalInvTransf;
                }
            }
        }
        

        for (unsigned int i = 0 ; i < pNode->mNumChildren ; i++)
        {
            readNodes(animTime, pNode->mChildren[i], globalTransf, nodeAnimMap, meshes);
        }
	}

    unsigned int Animator::FindPosition(float animTime, const aiNodeAnim* pNodeAnim)
    {    
        for (unsigned int i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
            if (animTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
                return i;
            }
        }
    
        assert(0);

        return 0;
    }

    unsigned int Animator::FindRotation(float animTime, const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumRotationKeys > 0);

        for (unsigned int i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
            if (animTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
                return i;
            }
        }
    
        assert(0);

        return 0;
    }

    unsigned int Animator::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumScalingKeys > 0);
        //cout <<  pNodeAnim->mNumScalingKeys;
        for (unsigned int i = 0 ; i <  pNodeAnim->mNumScalingKeys - 1; i++) {

                if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
                    return i;
                }
            
        }
    
        assert(0);

        return 0;
    }

    void Animator::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumPositionKeys == 1) {
            Out = pNodeAnim->mPositionKeys[0].mValue;
            return;
        }
            
        unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
        unsigned int NextPositionIndex = (PositionIndex + 1);
        assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
        float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
        const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }

    void Animator::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
	    // we need at least two values to interpolate...
        if (pNodeAnim->mNumRotationKeys == 1) {
            Out = pNodeAnim->mRotationKeys[0].mValue;
            return;
        }
    
        unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
        unsigned int NextRotationIndex = (RotationIndex + 1);
        assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
        float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;    
        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
        Out = Out.Normalize();
    }

    void Animator::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumScalingKeys == 1) {
            Out = pNodeAnim->mScalingKeys[0].mValue;
            return;
        }

        unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
        unsigned int NextScalingIndex = (ScalingIndex + 1);
        assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
        float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
        const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }
	
	glm::mat4 Animator::aiToGlm(aiMatrix4x4 m)
	{
		glm::vec4 vec1, vec2, vec3, vec4;
        /*
        vec1 = glm::vec4(matr.a1, matr.b1, matr.c1, matr.d1);
        vec2 = glm::vec4(matr.a2, matr.b2, matr.c2, matr.d2);
        vec3 = glm::vec4(matr.a3, matr.b3, matr.c3, matr.d3);
        vec4 = glm::vec4(matr.a4, matr.b4, matr.c4, matr.d4);
        
        vec1 = glm::vec4(matr.a1, matr.a2, matr.a3, matr.a4);
        vec2 = glm::vec4(matr.b1, matr.b2, matr.b3, matr.b4);
        vec3 = glm::vec4(matr.c1, matr.c2, matr.c3, matr.c4);
        vec4 = glm::vec4(matr.d1, matr.d2, matr.d3, matr.d4);
        */
         glm::mat4 retMatr = glm::mat4(m[0][0],  m[0][1], m[0][2], m[0][3],
    m[1][0], m[1][1], m[1][2], m[1][3],
    m[2][0], m[2][1], m[2][2], m[2][3],
    m[3][0], m[3][1], m[3][2], m[3][3]);		
        
		return (retMatr);
	}

    glm::mat4 Animator::aiToGlm(aiMatrix3x3 m)
    {
        glm::mat4 retMatr = glm::mat4(m[0][0],  m[0][1], m[0][2], 0.0f,
    m[1][0], m[1][1], m[1][2], 0.0f,
    m[2][0], m[2][1], m[2][2], 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
        return (retMatr);
    }