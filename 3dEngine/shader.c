#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include "fileread.h"



//creates shader programm
void CreateShader(shaderStruct* shader, const char* vertexPath, const char* fragmentPath)
{ 
  //reading shader files
  char *vertexCode = ReadFromFile(vertexPath);
  char *fragmentCode = ReadFromFile(fragmentPath);
  //cheking if files do exist
  if(vertexCode == NULL)
  {
    printf("CAN'T CREATE SHADER, VERTEX SHADER FILE IS NON EXISTENT!!");
    return;
  }
  if(fragmentCode == NULL)
  {
    printf("CAN'T CREATE SHADER, FRAGMENT SHADER FILE IS NON EXISTENT!!");
    return;
  }
  const char* vShaderCode = vertexCode;
  const char* fShaderCode = fragmentCode;
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  //creating vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(vertex, 512, NULL, infoLog);
    printf("ERROR SHADER VERTEX COMIPALTION_FAILED\n %s",infoLog);
  }

  //creating fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS,&success);
  if(!success)
  {
    glGetProgramInfoLog(fragment, 512, NULL, infoLog);
    printf("ERROR SHADER FRAGMENT COMIPALTION_FAILED\n %s",infoLog);
  }
  
  //linking the programm
  shader->ID = glCreateProgram();
  glAttachShader(shader->ID, vertex);
  glAttachShader(shader->ID, fragment);
  glLinkProgram(shader->ID);
  glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(shader->ID, 512, NULL, infoLog);
    printf("ERROR SHADER PROGRAM LINKING_FAILED\n %s",infoLog);
  }
  
  
//dealocating the memory
  free(vertexCode);
  free(fragmentCode);
  vShaderCode = NULL;
  fShaderCode = NULL;
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void UseShader(shaderStruct* shader)
{
  glUseProgram(shader->ID);
}

void SetBool(shaderStruct *shader, const char *name, bool value)
{
  glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}
void SetFloat(shaderStruct *shader, const char *name, float value)
{
  glUniform1f(glGetUniformLocation(shader->ID, name), value);
}
void SetInt(shaderStruct *shader, const char *name, int value)
{
  glUniform1i(glGetUniformLocation(shader->ID, name), value);
}
void DeleteShader(shaderStruct *shader)
{
  shader = NULL;
}

void SetMatrix4f(shaderStruct* shader, const char* name, mat4 matrix)
{
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, matrix[0]);
}
