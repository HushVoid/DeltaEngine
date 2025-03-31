#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "./include/cglm/cglm.h"
#include "./include/GL/glew.h"
#include "light.h"

typedef struct{
 unsigned int ID;
}shaderStruct;    

void CreateShader(shaderStruct* shader, const char* vertexPath, const char* fragmentPath);

void UseShader(shaderStruct* shader);

void SetShaderBool(shaderStruct* shader,const char* name, bool value);
void SetShaderFloat(shaderStruct* shader,const char* name, float value);
void SetShaderInt(shaderStruct* shader, const char* name, int value);
void SetShaderFloat3(shaderStruct* shader, const char* name, vec3 floats);
void SetShaderFloat4(shaderStruct* shader, const char* name, vec4 floats);
void SetShaderMatrix3f(shaderStruct* shader, const char* name, mat3 matrix);
void SetShaderMatrix4f(shaderStruct* shader, const char* name, mat4 matrix);

//light

void SetDirLightStruct(shaderStruct* shader, const char* name, DirLight light);
void SetPointLightStruct(shaderStruct* shader, const char* name, PointLight light);
void SetSpotLightStruct(shaderStruct* shader, const char* name, SpotLight light);

void DeleteShader(shaderStruct* shader);


#endif
