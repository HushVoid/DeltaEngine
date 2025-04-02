#include "model.h"
#include <stdio.h>


void ModelInit(Model *model, char *path)
{
 model->meshes = dynlistInit(sizeof(Mesh), 0);
 LoadModel(model, path);
}


void DrawModel(Model *model, shaderStruct *shader)
{
  for(int i = 0; i < dynlistSize(model->meshes); i++)
  {
    Draw(&model->meshes[i], shader);
  }
}

void LoadModel(Model *model, char *path)
{
 const struct aiScene* scene = aiImportFile(path,aiProcess_Triangulate | aiProcess_FlipUVs);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    printf("ERROR ASSIMP %s", aiGetErrorString());
    return;
  }
  ExtractDir(path, model->directory);
  ProcessNode(model, scene->mRootNode, scene);
}  
void ProcessNode(Model *model, struct aiNode *Node, const struct aiScene *scene)
{
  for(unsigned int i = 0; i < Node->mNumMeshes; i++)
  {
    struct aiMesh *mesh = scene->mMeshes[Node->mMeshes[i]];
    model->meshes = dynlistPush(model->meshes, ProcessMesh(model, mesh, scene));
  }
  for(unsigned int i = 0; i < Node->mNumChildren; i++)
  {
    ProcessNode(model, Node, scene);
  }
}


void ExtractDir(const char *path, char *dir)
{
    const char *last_slash = strrchr(path, '/');
    
    if (last_slash != NULL) 
    {
        size_t length = last_slash - path;
        strncpy(dir, path, length);
        dir[length] = '\0'; 
    }
    else 
    {
        strcpy(dir, ".");
    }
}
