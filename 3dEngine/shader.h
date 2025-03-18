#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "src/include/cglm/cglm.h"
#include "src/include/GL/glew.h"

typedef struct{
 unsigned int ID;
}shaderStruct;    

void CreateShader(shaderStruct* shader, const char* vertexPath, const char* fragmentPath);

void UseShader(shaderStruct* shader);

void SetBool(shaderStruct* shader,const char* name, bool value);
void SetFloat(shaderStruct* shader,const char* name, float value);
void SetInt(shaderStruct* shader, const char* name, int value);
void SetMatrix4f(shaderStruct* shader, const char* name, mat4 matrix);
void DeleteShader(shaderStruct* shader);


#endif
