#include "model.h"
#include "dynlist.h"
#include "include/assimp/material.h"
#include "include/assimp/types.h"
#include "include/cglm/vec2.h"
#include "include/cglm/vec3.h"
#include "mesh.h"
#include "include/STBI/stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void ModelInit(Model *model, char *path)
{

 model->texturesLoaded = dynlistInit(sizeof(Texture), 4);
 model->meshes = dynlistInit(sizeof(Mesh), DEFAULT_INIT_CAPACITY);
 LoadModel(model, path);
}


void DrawModel(Model *model, shaderStruct *shader) 
{
    for(size_t i = 0; i < model->meshes->size; i++) 
    {
        Mesh* mesh = dynlistAt(model->meshes, i);
        if(mesh) 
        {
            Draw(mesh, shader);
        }
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
  aiReleaseImport(scene);
}  
void ProcessNode(Model *model, struct aiNode *node, const struct aiScene *scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++) 
    {
        struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        Mesh* structMesh = ProcessMesh(model, mesh, scene);
        if(structMesh) 
        {
            dynlistPush(model->meshes, structMesh);
        }
        // DON'T free structMesh here - it's owned by model->meshes now
    }
    
    for(unsigned int i = 0; i < node->mNumChildren; i++) 
    {
        ProcessNode(model, node->mChildren[i], scene);
    }
}

Mesh* ProcessMesh(Model *model, struct aiMesh *mesh, const struct aiScene *scene)
{
    Mesh *rMesh = (Mesh*)calloc(1, sizeof(Mesh));
    if(!rMesh) return NULL;

    rMesh->vertices = dynlistInit(sizeof(Vertex), mesh->mNumVertices);
    rMesh->indices = dynlistInit(sizeof(unsigned int), mesh->mNumFaces * 3);
    rMesh->textures = dynlistInit(sizeof(Texture), 4); // Default texture capacity

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
    if (mesh->mNormals)
    { 
    pos[0] = mesh->mNormals[i].x;
    pos[1] = mesh->mNormals[i].y;
    pos[2] = mesh->mNormals[i].z;
    glm_vec3_copy(pos, vertex.normal);
    } else 
    {
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, vertex.normal); // Дефолтное значение
    }
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
    dynlistPush(rMesh->vertices, &vertex);
  }
  for(unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    //processing indices for each face of a mesh
    struct aiFace face = mesh->mFaces[i];
    for(unsigned int j = 0; j < face.mNumIndices; j++)
     dynlistPush(rMesh->indices, &face.mIndices[j]); 
  }
  char execpath[MAX_PATH];
  if(GetModuleFileName(NULL,execpath, MAX_PATH) != 0)
  {
    char* lastSlash = strrchr(execpath, '\\');
    if(lastSlash != NULL) *lastSlash = '\0';        
  }
  if(mesh->mMaterialIndex > 0) 
  {
      struct aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
      if(aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE) == 0)
      {
         char missingPath[256];
         strcpy_s(missingPath, sizeof(missingPath), execpath);
         strcat_s(missingPath, sizeof(missingPath), MISSING_TEXTURE_PATH);
         Texture texture;
         texture.id = TextureFromFile("", missingPath);
         strcpy(texture.type, "texture_diffuse");
         strcpy(texture.path, MISSING_TEXTURE_PATH);
         dynlistPush(rMesh->textures, &texture);
         dynlistPush(model->texturesLoaded, &texture);
      }
      else
      {

        // Load diffuse maps
        dynlist_t* diffuseMaps = LoadMaterialTextures(model, material, 
                                   aiTextureType_DIFFUSE, "texture_diffuse");
        if(diffuseMaps)
        {
            // Transfer ownership of textures to rMesh->textures
            for(size_t i = 0; i < diffuseMaps->size; i++)
            {
                Texture* tex = dynlistAt(diffuseMaps, i);
                if(tex) dynlistPush(rMesh->textures, tex);
            }
            // Free only the container, not the textures
            dynlistFreeContainerOnly(diffuseMaps);
        }
        
        
        // Load specular maps
        dynlist_t* specularMaps = LoadMaterialTextures(model, material,
                                    aiTextureType_SPECULAR, "texture_specular");
        if(specularMaps)
        {
            // Transfer ownership of textures to rMesh->textures
            for(size_t i = 0; i < specularMaps->size; i++) 
            {
                Texture* tex = dynlistAt(specularMaps, i);
                if(tex) dynlistPush(rMesh->textures, tex);
            }
            // Free only the container, not the textures
            dynlistFreeContainerOnly(specularMaps);
        }
        
      }
  }
  else 
  {
         char missingPath[256];
         strcpy_s(missingPath, sizeof(missingPath), execpath);
         strcat_s(missingPath, sizeof(missingPath), MISSING_TEXTURE_PATH);
         Texture texture;
         texture.id = TextureFromFile("", missingPath);
         strcpy(texture.type, "texture_diffuse");
         strcpy(texture.path, MISSING_TEXTURE_PATH);
         dynlistPush(rMesh->textures, &texture);
         dynlistPush(model->texturesLoaded, &texture);
  }
  SetupMesh(rMesh);
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
  char fullPath[512] = "\0";
  if(strcmp(directory, MISSING_TEXTURE_PATH) == 0)
  {  
    printf("Loading model without texture\n");
  }
  else
  { 
    strcat_s(fullPath, sizeof(fullPath), directory);
    strcat_s(fullPath, sizeof(fullPath), "\\");
    strcat_s(fullPath, sizeof(fullPath), path);
  }

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
    printf("Texture failed to load at path: %s \n", fullPath);
    stbi_image_free(data);
    data = stbi_load(MISSING_TEXTURE_PATH, &width, &height, &nrComponents, 0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);


  }
  fullPath[0] = '\0';
  return textureID;

}
dynlist_t* LoadMaterialTextures(Model *model, struct aiMaterial *mat, enum aiTextureType type, char* typeName) 
{
    dynlist_t* textures = dynlistInit(sizeof(Texture), DEFAULT_INIT_CAPACITY);
    unsigned int texCount = aiGetMaterialTextureCount(mat, type);
    
    for(unsigned int i = 0; i < texCount; i++) 
      {
        struct aiString str;
        aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);
        bool skip = false;
        
        for(size_t j = 0; j < model->texturesLoaded->size; j++) 
        {
            Texture* texture = dynlistAt(model->texturesLoaded, j);
            if(texture && strcmp(texture->path, str.data) == 0)
            {
                dynlistPush(textures, texture);
                skip = true;
                break;
            }
        }
        
        if(!skip) 
        {
            Texture texture;
            texture.id = TextureFromFile(str.data, model->directory);
            strcpy_s(texture.type, 63, typeName);
            texture.type[63] = '\0';
            strcpy_s(texture.path, 511, str.data);
            texture.path[511] = '\0';
            dynlistPush(textures, &texture);
            dynlistPush(model->texturesLoaded, &texture);
        }
    }
    return textures;
}
void DeleteModel(Model* model) 
{
    for(size_t i = 0; i < model->meshes->size; i++)
    {
        Mesh* mesh = dynlistAt(model->meshes, i);
        if(mesh) 
        {
            DeleteMesh(mesh);
        }
    }
    if(model->texturesLoaded) 
    {
        dynlistFree(model->texturesLoaded);
    }
}
