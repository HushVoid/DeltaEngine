#ifndef GPU_LIGHT_H
#define GPU_LIGHT_H

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

#include "include/cglm/cglm.h"
#include "include/GL/glew.h"
#include "macro.h"
#include "nodes/light_node.h"

typedef enum
{
  LIGHT_TYPE_DIR,
  LIGHT_TYPE_POINT,
  LIGHT_TYPE_SPOT
} LightType;


typedef struct {
    vec4 position;       // .w = intensity
    vec4 color_diffuse;  // RGB
    vec4 attenuation; // constant + linear + quadratic + padding
    int id;
} GPUPointLight;

typedef struct {
  vec4 direction; //.w = intensity;
  vec4 color; // .w = padding; 
} GPUDirLight;


typedef struct {
  vec4 direction; //.w = innerCutOff;
  vec4 color; // .w = outerCutOff;
  vec4 position; //.w = intensity;
  int id;
} GPUSpotLight;

//initializing from nodes
GPUPointLight* GPUPointLightCreate(PointLightNode* node);
GPUDirLight* GPUDirLightCreate(DirectionalLightNode* node);
GPUSpotLight* GPUSpotLightCreate(SpotLightNode* node);


void DirLight_UploadToUBO(GLuint ubo, GPUDirLight* light); 
void PointLight_UploadToUBO(GLuint ubo, GPUPointLight* light, unsigned int index); 
void SpotLight_UploadToUBO(GLuint ubo, GPUSpotLight* light, unsigned int index); 

#endif // !GPU_LIGHT_H
