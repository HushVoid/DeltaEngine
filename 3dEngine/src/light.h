#ifndef LIGHT_H
#define LIGHT_H
#define DEFAULT_SPECULAR ((const float[]){1.0f,1.0f,1.0f})
#define DEFAULT_DIFFUSE ((const float[]){0.5f,0.5f,0.5f})
#define DEFAULT_AMBIENT ((const float[]){0.2f,0.2f,0.2f})

#define DEFAULT_PL_CONSTANT 1.0;
#define DEFAULT_PL_LINEAR 0.09;
#define DEFAULT_PL_QUADRATIC 0.032;

#include "macro.h"
#include "include/cglm/cglm.h"


typedef struct 
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
} DirLight;

typedef struct 
{

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
} PointLight;

typedef struct 
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float cutOff;
  float outerCutOff;
} SpotLight;

void InitDefaultDirLight(DirLight *light, vec3 direction);
void InitDefaultPointLight(PointLight *light, vec3 position, float distance);
void InitDefaultSpotLight(SpotLight *light, vec3 position, vec3 direction, float cutOff, float outerCutOff);

#endif // !LIGHT_H
