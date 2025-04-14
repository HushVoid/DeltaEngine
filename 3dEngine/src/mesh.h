#ifndef MESH_H
#define MESH_H

#include "include/cglm/cglm.h"
#include "include/GL/glew.h"
#include "dynlist.h"
#include "shader.h"

typedef struct
{
  vec3 position;
  vec3 normal;
  vec2 TexCoords;
}Vertex;

typedef struct
{
  unsigned int id;
  char type[64];
  char path[512];
} Texture;

typedef struct
{
  dynlist_t *vertices; //dynlist<Vertex>
  dynlist_t  *indices; //dynlist<unsigned int>
  dynlist_t *textures; //dynlist <Texture>
  
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;
} Mesh;

void MeshInit(Mesh *mesh,dynlist_t *vertices, dynlist_t *indices, dynlist_t *textures);
void Draw(Mesh* mesh,shaderStruct *shader);
void SetupMesh(Mesh *mesh);
void DeleteMesh(Mesh *mesh);

#endif // !MESH_Hj
