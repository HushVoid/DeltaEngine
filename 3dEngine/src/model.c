#include "model.h"
#include "dynlist.h"
#include "include/assimp/material.h"
#include "include/assimp/types.h"
#include "include/cglm/vec2.h"
#include "include/cglm/vec3.h"
#include "mesh.h"
#include "include/STBI/stb_image.h"
#include <stdio.h>
#include <string.h>


void ModelInit(Model *model, char *path)
{
 model->texturesLoaded = dynlistInit(sizeof(Texture), 2);
 model->meshes = dynlistInit(sizeof(Mesh), 1);
 LoadModel(model, path);
}


void DrawModel(Model *model, shaderStruct *shader)
{
  for(int i = 0; i < dynlistSize(model->meshes); i++)
  {
    //Drawing each mesh of a model
    Draw(&model->meshes[i], shader);
  }
}

void LoadModel(Model *model, char *path)
{
//importing file
 const struct aiScene* scene = aiImportFile(path,aiProcess_Triangulate | aiProcess_FlipUVs);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    printf("ERROR ASSIMP %s", aiGetErrorString());
    return;
  }
 //Assiging directory
  ExtractDir(path, model->directory);
  ProcessNode(model, scene->mRootNode, scene);
}  
void ProcessNode(Model *model, struct aiNode *Node, const struct aiScene *scene)
{
  //Processing each mesh of a node
  for(unsigned int i = 0; i < Node->mNumMeshes; i++)
  {
    struct aiMesh *mesh = scene->mMeshes[Node->mMeshes[i]];
    model->meshes = dynlistPush(model->meshes, ProcessMesh(model, mesh, scene));
  }
  for(unsigned int i = 0; i < Node->mNumChildren; i++)
  {
    //If node has children calling this function recursively
    ProcessNode(model, Node, scene);
  }
}

Mesh* ProcessMesh(Model *model, struct aiMesh *mesh, const struct aiScene *scene)
{
  //creating mesh in memory
  Mesh* rMesh = malloc(sizeof(Mesh));
  //initializing dynlists 
  Vertex* vertices = dynlistInit(sizeof(Vertex), 1);
  unsigned int* indices = dynlistInit(sizeof(unsigned int), 1);
  Texture* textures = dynlistInit(sizeof(Texture), 1);

  for(unsigned int i = 0; i < mesh->mNumVertices; i++)
  {
    //Processing vertices
    Vertex vertex;
    //vertex position
    vec3 pos;
    pos[0] = mesh->mVertices[i].x;
    pos[1] = mesh->mVertices[i].y;
    pos[2] = mesh->mVertices[i].z;
    glm_vec3_copy(pos, vertex.position);
    //vertex normals
    pos[0] = mesh->mNormals[i].x;
    pos[1] = mesh->mNormals[i].y;
    pos[2] = mesh->mNormals[i].z;
    glm_vec3_copy(pos, vertex.normal);
    //vertex texture coordinates
    if(mesh->mTextureCoords[0])
    {
      vec2 texPos;
      texPos[0] = mesh->mTextureCoords[0][i].x;
      texPos[1] = mesh->mTextureCoords[0][i].y;
      glm_vec2_copy(texPos, vertex.TexCoords);
    }
    else
    {
      glm_vec2_copy((vec2){0.0f,0.0f}, vertex.TexCoords);
    }
    //pushing them into list
    vertices = dynlistPush(vertices, &vertex);
  }
  for(unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    //processing indices for each face of a mesh
    struct aiFace face = mesh->mFaces[i];
    for(unsigned int j = 0; j < face.mNumIndices; j++)
       indices = dynlistPush(indices, &face.mIndices[i]); 
  }  
  if(mesh->mMaterialIndex >= 0)
  {
    //Loading textures from materials
    struct aiMaterial *material  = scene->mMaterials[mesh->mMaterialIndex];
    Texture* diffuseMaps = LoadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures = dynlistPushArray(textures, diffuseMaps, dynlistSize(diffuseMaps));
    Texture* specualMaps = LoadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
    textures = dynlistPushArray(textures, specualMaps, dynlistSize(specualMaps));
  }
  //initializing mesh
  MeshInit(rMesh, vertices, indices, textures);
  return rMesh;
}

//Extracting directory from fullpath
void ExtractDir(const char *path, char *dir)
{
    const char *last_slash = strrchr(path, '\\');
    
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
unsigned int TextureFromFile(const char *path, const char *directory)
{
  //concatinating full path
  char fullPath[512];
  strcat_s(fullPath, sizeof(fullPath), directory);
  strcat_s(fullPath, sizeof(fullPath), "\\");
  strcat_s(fullPath, sizeof(fullPath), path);

  //generating texture
  unsigned int textureID;
  glGenTextures(1, &textureID);
  int width, height, nrComponents;
  unsigned char *data = stbi_load(fullPath, &width, &height, &nrComponents, 0);
  //creating texture if it's loaded
  if(data) 
  {
    GLenum format;
    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    printf("Texture failed to load at path: %s", fullPath);
    stbi_image_free(data);
  }
  return textureID;

}
Texture* LoadMaterialTextures(Model *model, struct aiMaterial *mat, enum aiTextureType type, char* typeName)
{
  Texture* textures = dynlistInit(sizeof(Texture), 1);
  for(unsigned int i = 0; i < aiGetMaterialTextureCount(mat, type); i++)
  {
    struct aiString str;
    aiGetMaterialTexture(mat,type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);
    bool skip = false;
    for(unsigned int j = 0; j < dynlistSize(model->texturesLoaded); j++)
    {
      if(strcmp(model->texturesLoaded[j].path, str.data) == 0)
      {
        textures = dynlistPush(textures, &model->texturesLoaded[j]);
        skip = true;
        break;
      }
    }
    if(!skip)
    {
      Texture texture;
      texture.id = TextureFromFile(str.data, model->directory);
      strcpy_s(texture.type, 64, typeName);
      strcpy_s(texture.path, 512, str.data);
      textures = dynlistPush(textures, &texture);
      model->texturesLoaded = dynlistPush(model->texturesLoaded, &texture);
    }
    return textures;

  }
}
void DeleteModel(Model* model)
{
  for(int i = 0; i < dynlistSize(model->meshes); i++)
  {
    //Deleting mesh memory
    DeleteMesh(&model->meshes[i]);
  }
  if(model->texturesLoaded)
  {
    dynlistFree(model->texturesLoaded);
  }
  //Deleting char array memory
}
