#include "light.h"
#include "macro.h"
#include "include/cglm/cglm.h"


void InitDefaultDirLight(DirLight *light, vec3 direction)
{
 for(int i = 0; i < 3; i++)
  {
   light->direction[i] = direction[i]; 
   light->ambient[i] = DEFAULT_AMBIENT[i]; 
   light->diffuse[i] = DEFAULT_DIFFUSE[i]; 
   light->specular[i] = DEFAULT_SPECULAR[i]; 
  }
}
void InitDefaultPointLight(PointLight *light, vec3 position, float distance)
{
 for(int i = 0; i < 3; i++)
  {
   light->position[i] = position[i]; 
   light->ambient[i] = DEFAULT_AMBIENT[i]; 
   light->diffuse[i] = DEFAULT_DIFFUSE[i]; 
   light->specular[i] = DEFAULT_SPECULAR[i]; 
  }
  light->constant = DEFAULT_PL_CONSTANT;
  light->linear = DEFAULT_PL_LINEAR;
  light->linear /= distance; // i don't know why but i can't do the division in the upper line C is strange :d
  light->quadratic = DEFAULT_PL_QUADRATIC;
  light->linear /= distance;
}

//Passing in angles but asinging cosine of angle 
void InitDefaultSpotLight(SpotLight *light, vec3 position, vec3 direction, float cutOff, float outerCutOff)
{
 for(int i = 0; i < 3; i++)
  {
   light->direction[i] = direction[i]; 
   light->ambient[i] = DEFAULT_AMBIENT[i]; 
   light->diffuse[i] = DEFAULT_DIFFUSE[i]; 
   light->specular[i] = DEFAULT_SPECULAR[i]; 
  }
  light->cutOff = cos(DEG2RAD(cutOff));  
  light->outerCutOff = cos(DEG2RAD(outerCutOff));
}
