#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

#include "shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



#include <string>
#include <vector>
using namespace std;

struct Vertex {
   Vertex() : Weights(glm::vec3(0.0, 0.0, 0.0)), JointIds(glm::uvec3(0,0,0)) {}
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    //joint id
    glm::uvec3 JointIds;
    //weights
    glm::vec3 Weights;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

struct Weight {
    unsigned int vertexId;
    float weight;
};

struct Bone {
    aiString name;
    unsigned int numWeights;
    glm::mat4 offsetMat;
    glm::mat4 finalTransf;
    Weight* weights;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    vector<Bone>         bones;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, vector<Bone> bones);

    // render the mesh
    void Draw(Shader shader);

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
   
};
#endif
