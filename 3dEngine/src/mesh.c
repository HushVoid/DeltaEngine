#include "include/cglm/cglm.h"
#include "include/GL/glew.h"
#include "dynlist.h"
#include "shader.h"
#include <string.h>
#include "mesh.h"


void MeshInit(Mesh *mesh,dynlist_t *vertices, dynlist_t *indices, dynlist_t *textures)
{
  //initializing dynlists
  mesh->vertices = vertices;
  mesh->indices = indices;
  mesh->textures = textures;
  SetupMesh(mesh);
} 
//Setuping GPU stuff 
void SetupMesh(Mesh *mesh)
{
  
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);
  
    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices->size * mesh->vertices->elemSize, mesh->vertices->items, GL_STATIC_DRAW);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices->size * mesh->indices->elemSize, 
                 mesh->indices->items, GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0); 
}
void Draw(Mesh *mesh,shaderStruct *shader)
{
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  for(size_t i = 0; i < mesh->textures->size; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    char number[3];
    char name[30];
    Texture* currtexture = dynlistAt(mesh->textures, i);
    strcpy_s(name, sizeof(name) , currtexture->type);
    if(strcmp(name, "texture_diffuse"))
      snprintf(number, sizeof(number), "%d", diffuseNr++);
    if(strcmp(name, "texture_specular"))
      snprintf(number, sizeof(number), "%d", diffuseNr++);
    char result[50] = "\0";
    strcat_s(result , sizeof(result), name);
    strcat_s(result , sizeof(result), number);
    printf("%s\n", result);
    SetShaderInt(shader, result, i);
    glBindTexture(GL_TEXTURE_2D, currtexture->id);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(mesh->VAO);
  glDrawElements(GL_TRIANGLES, mesh->indices->size, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
void DeleteMesh(Mesh *mesh)
{
  if(!mesh) return; 
  //CPU resources
  if(mesh->vertices)
    dynlistFree(mesh->vertices);
  if(mesh->indices)
    dynlistFreeContainerOnly(mesh->textures); 
  if(mesh->textures)
    dynlistFree(mesh->indices);

  //GPU resources 
  if(mesh->VAO != 0)
  {
    glDeleteVertexArrays(1, &mesh->VAO);
    mesh->VAO = 0;
  }  
  if(mesh->VBO != 0)
  {
    glDeleteBuffers(1, &mesh->VBO);
    mesh->VBO = 0;
  }  
  if(mesh->EBO != 0)
  {
    glDeleteBuffers(1, &mesh->EBO);
    mesh->EBO = 0;
  }  
}
