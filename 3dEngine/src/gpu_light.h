#ifndef GPU_LIGHT_H
#define GPU_LIGHT_H

#include "include/cglm/cglm.h"
#include "include/GL/glew.h"
#include "macro.h"
#include "light_node.h"

typedef struct {
    vec4 position;       // .w = intensity
    vec4 color_diffuse;  // RGB + attenuation
    vec4 attenuation; // constant + linear + quadratic + padding
} GPUPointLight;

typedef struct {
  vec4 direction; //.w = intensity;
  vec4 color; // .w = padding; 
} GPUDirLight;
typedef struct {
  vec4 direction; //.w = innerCutOff;
  vec4 color; // .w = outerCutOff;
  vec4 fade; // .x = theta .y = epsilon .w = intensity  
} GPUSpotLight;

//initializing from nodes
void InitGPUPointLight(PointLightNode* node, GPUPointLight* light);
void InitGPUDirLight(DirectionalLightNode* node, GPUDirLight* light);
void InitGPUSpotLight(SpotLightNode* node, GPUSpotLight* light);


void DirLight_UploadToUBO(GLuint ubo, GPUDirLight* light); 
void PointLight_UploadToUBO(GLuint ubo, GPUPointLight* light); 
void SpotLight_UploadToUBO(GLuint ubo, GPUSpotLight* light); 

#endif // !GPU_LIGHT_H
