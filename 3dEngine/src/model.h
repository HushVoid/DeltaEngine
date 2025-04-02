#ifndef MODEL_H
#define MODEL_H

#include "include/cglm/cglm.h"
#include "include/GL/glew.h"
#include "dynlist.h"
#include "shader.h"
#include "mesh.h"
#include "include/assimp/cimport.h"
#include "include/assimp/scene.h"
#include "include/assimp/postprocess.h"


typedef struct
{
  Mesh *meshes;
  char directory[256];
} Model;

void ModelInit(Model *model, char *path);
void LoadModel(Model *model, char *path);
void DrawModel(Model *model, shaderStruct *shader);
void ProcessNode(Model *model, struct aiNode *Node, const struct aiScene *scene); 
Mesh* ProcessMesh(Model *model, struct aiMesh *mesh, const struct aiScene *scene);
Texture* LoadMaterialTextures(Model *model, struct aiMaterial *mat, enum aiTextureType type, char* typeName);
void ExtractDir(const char *path, char *dir);


#endif 
