#include "include/cglm/cglm.h"
#include "include/GL/glew.h"
#include "dynlist.h"
#include "shader.h"
#include <string.h>
#include "mesh.h"


void MeshInit(Mesh *mesh,Vertex *vertices, unsigned int *indices, Texture *textures)
{
  //initializing dynlists
 mesh->vertices = dynlistInit(sizeof(Vertex), 1);
 mesh->indices = dynlistInit(sizeof(Vertex), 1);
 mesh->textures = dynlistInit(sizeof(Texture),1);
  
 //pushing coresponding parameters
 mesh->vertices = dynlistPushArray(mesh->vertices, vertices, dynlistSize(vertices));
 mesh->indices = dynlistPushArray(mesh->indices, indices, dynlistSize(indices));
 mesh->vertices = dynlistPushArray(mesh->vertices, textures, dynlistSize(textures));

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

    glBufferData(GL_ARRAY_BUFFER, dynlistSize(mesh->vertices) * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dynlistSize(mesh->indices) * sizeof(unsigned int), 
                 &mesh->indices[0], GL_STATIC_DRAW);

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
  for(int i = 0; i < dynlistSize(mesh->textures); i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    char number[3];
    char name[30];
    strcpy_s(name, sizeof(name) , mesh->textures[i].type);
    if(strcmp(name, "texture_diffuse"))
      snprintf(number, sizeof(number), "%d", diffuseNr++);
    if(strcmp(name, "texture_specular"))
      snprintf(number, sizeof(number), "%d", diffuseNr++);
    char result[50] = "material.";
    strcat_s(result , sizeof(result), name);
    strcat_s(result , sizeof(result), number);
    SetShaderInt(shader, result, i);
    glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(mesh->VAO);
  glDrawElements(GL_TRIANGLES, dynlistSize(mesh->indices), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
void DeleteMesh(Mesh *mesh)
{
  if(!mesh) return; 
  //CPU resources
  if(mesh->vertices)
    dynlistFree(mesh->vertices);
  if(mesh->indices)
    dynlistFree(mesh->textures); 
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
