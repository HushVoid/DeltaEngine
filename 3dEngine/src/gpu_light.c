#include "gpu_light.h"
#include "macro.h"


GPUPointLight* GPUPointLightCreate(PointLightNode* node)
{
  GPUPointLight* light = calloc(1, sizeof(GPUPointLight));
  glm_vec4_copy3(node->base.transform.position, light->position);
  light->position[3] = node->intencity;
  glm_vec4_copy3(node->light.color, light->color_diffuse);
  light->attenuation[0] = node->light.constant;
  light->attenuation[1] = node->light.linear;
  light->attenuation[2] = node->light.quadratic;
  light->id = node->id;
  return light;
}
GPUDirLight* GPUDirLightCreate(DirectionalLightNode* node)
{
  GPUDirLight* light = calloc(1, sizeof(GPUDirLight));
  glm_vec4_copy3(node->light.direction, light->direction);
  light->direction[3] = node->intencity;
  glm_vec4_copy3(node->light.color, light->color);
  return light;
}
GPUSpotLight* GPUSpotLightCreate(SpotLightNode* node)
{
 GPUSpotLight* light = calloc(1, sizeof(GPUSpotLight)); 
 glm_vec4_copy3(node->light.direction, light->direction);
 light->direction[3] = cos(DEG2RAD(node->light.cutOff));
 glm_vec4_copy3(node->light.color, light->color);
 light->color[3] = cos(DEG2RAD(node->light.outerCutOff));
 glm_vec4_copy3(node->base.transform.position, light->position);
 light->position[3] = node->intencity;
 light->id = node->id;
 return light;
}
void DirLight_UploadToUBO(GLuint ubo, GPUDirLight* light)
{
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPUDirLight), light);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void PointLight_UploadToUBO(GLuint ubo, GPUPointLight* light, unsigned int index)
{
  size_t offset = sizeof(GPUDirLight) + index * sizeof(GPUPointLight);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GPUPointLight), light);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void SpotLight_UploadToUBO(GLuint ubo, GPUSpotLight* light, unsigned int index)
{
  size_t offset = sizeof(GPUDirLight) + MAX_POINT_LIGHTS * sizeof(GPUPointLight) + index * sizeof(GPUSpotLight);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GPUSpotLight), light);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
