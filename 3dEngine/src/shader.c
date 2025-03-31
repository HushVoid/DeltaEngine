#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileread.h"
#include "light.h"

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

void SetShaderBool(shaderStruct *shader, const char *name, bool value)
{
  glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}
void SetShaderFloat(shaderStruct *shader, const char *name, float value)
{
  glUniform1f(glGetUniformLocation(shader->ID, name), value);
}
void SetShaderInt(shaderStruct *shader, const char *name, int value)
{
  glUniform1i(glGetUniformLocation(shader->ID, name), value);
}
void DeleteShader(shaderStruct *shader)
{
  shader = NULL;
}

void SetShaderFloat3(shaderStruct* shader, const char* name, vec3 floats)
{
  glUniform3f(glGetUniformLocation(shader->ID, name),floats[0], floats[1], floats[2]);
}
void SetShaderFloat4(shaderStruct* shader, const char* name, vec4 floats)
{
  glUniform4f(glGetUniformLocation(shader->ID, name),floats[0], floats[1], floats[2], floats[3]);
}
void SetShaderMatrix4f(shaderStruct* shader, const char* name, mat4 matrix)
{
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, matrix[0]);
}
void SetShaderMatrix3f(shaderStruct* shader, const char* name, mat3 matrix)
{
  glUniformMatrix3fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, matrix[0]);
}


//Setting light structs 

void SetDirLightStruct(shaderStruct* shader, const char* name, DirLight light)
{
 char variable[30];
 //setting main light components
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".ambient");  
 SetShaderFloat3(shader, variable, light.ambient);
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".diffuse");  
 SetShaderFloat3(shader, variable, light.diffuse);
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".specular");  
 SetShaderFloat3(shader, variable, light.specular);

//setting direction
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".direction");
 SetShaderFloat3(shader, variable, light.direction);
}


void SetPointLightStruct(shaderStruct* shader, const char* name, PointLight light)
{ 
 //setting main light components
 char variable[30];
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".ambient");  
 SetShaderFloat3(shader, variable, light.ambient);
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".diffuse");  
 SetShaderFloat3(shader, variable, light.diffuse);
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".specular");  
 SetShaderFloat3(shader, variable, light.specular);

//setting position
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".position");
 SetShaderFloat3(shader, variable, light.position);

//setting point light atributes
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".constant");
 SetShaderFloat(shader, variable, light.constant);
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".linear");
 SetShaderFloat(shader, variable, light.linear);
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".quadratic");
 SetShaderFloat(shader, variable, light.quadratic);
}

void SetSpotLightStruct(shaderStruct* shader, const char* name, SpotLight light)
{
 //setting main light components
 char variable[30];
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".ambient");  
 SetShaderFloat3(shader, variable, light.ambient);
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".diffuse");  
 SetShaderFloat3(shader, variable, light.diffuse);
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".specular");  
 SetShaderFloat3(shader, variable, light.specular);

//setting position
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".position");
 SetShaderFloat3(shader, variable, light.position);

//setting direction
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".direction");
 SetShaderFloat3(shader, variable, light.direction);

//setting spot light atributes
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".cutOff");
 SetShaderFloat(shader, variable, light.cutOff);
 variable[0] = '\0';
 strcat_s(variable, sizeof(variable), name);
 strcat_s(variable, sizeof(variable), ".outerCutOff");
 SetShaderFloat(shader, variable, light.outerCutOff);
}
