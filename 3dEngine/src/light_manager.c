#include "light_manager.h"

LightManager* LightManagerCreate()
{
  LightManager* manager = calloc(1, sizeof(LightManager));
  manager->dirLight = NULL;
  manager->spotLights = dynlistInit(sizeof(GPUSpotLight*), 10);
  manager->pointLights = dynlistInit(sizeof(GPUPointLight*), 10);
  manager->spotLightsCount = 0;
  manager->pointLightsCount = 0;
  manager->nextLightid = 0;
  return manager;
}

void LMRemoveDLight(LightManager* lm, GLuint ubo)
{
 free(lm->dirLight); 
 lm->dirLight = NULL;
 UpdateUBOAfterRemoval(lm, ubo, LIGHT_TYPE_DIR, 0);
}
void LMRemoveSpotIndex(LightManager* lm, GLuint ubo, int index)
{
  dynlistDeleteAt(lm->spotLights, index);
  lm->spotLightsCount--;
  UpdateUBOAfterRemoval(lm, ubo, LIGHT_TYPE_SPOT, index);
}
void LMRemovePLightIndex(LightManager* lm, GLuint ubo, int index)
{
  dynlistDeleteAt(lm->pointLights, index);
  lm->pointLightsCount--;
  UpdateUBOAfterRemoval(lm, ubo, LIGHT_TYPE_POINT, index);
}

int LightManagerFindHigestLightID(LightManager* lm)
{
  if(!lm)
  {
    printf("LightManagerFindHigestLightID: light manager is invalid \n");
    return -1;
  }
  int max = -1;
  for(int i = 0; i < lm->pointLights->size; i++)
  {
    GPUPointLight* light = *(GPUPointLight**)dynlistAt(lm->pointLights, i);
    if(light->id > max)
      max = light->id;
  }
  for(int i = 0; i < lm->spotLights->size; i++)
  {
    GPUSpotLight* light = *(GPUSpotLight**)dynlistAt(lm->spotLights, i);
    if(light->id > max)
      max = light->id;
  }
  if(max > 0)
  {
    lm->nextLightid = max + 1;
  }
  return max;
}


int LightManagerFindLightByID(LightManager* lm, int id, LightType type)
{
  if(!lm)
  {
    printf("LightManagerFindLightByID: Lightmanager is invalid\n");
    return -1;
  }
  int index = -1;
  switch(type)
  {
    case LIGHT_TYPE_SPOT:
      for(int i = 0; i < lm->spotLights->size; i++)
      {
        GPUSpotLight* light = *(GPUSpotLight**)dynlistAt(lm->spotLights, i);
        if(light->id == id)
        {
          index = i;
          break;
        }
      }
    case LIGHT_TYPE_POINT:
      for(int i = 0; i < lm->pointLights->size; i++)
      {
        GPUPointLight* light = *(GPUPointLight**)dynlistAt(lm->pointLights, i);
        if(light->id == id)
        {
          index = i;
          break;
        }
      }
    case LIGHT_TYPE_DIR:
      return 0;
  }
  return index;
}
void UpdateUBOAfterRemoval(LightManager* lm, GLuint ubo, LightType type, int removedIndex) 
{
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    size_t baseOffset = sizeof(GPUDirLight);
    size_t lightSize = 0;
    int* count = NULL;

    switch (type) {
        case LIGHT_TYPE_DIR: 
            GPUDirLight empty = {0};
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPUDirLight), &empty);
            return;
        case LIGHT_TYPE_POINT:
            lightSize = sizeof(GPUPointLight);
            count = &lm->pointLightsCount;
            break;
        case LIGHT_TYPE_SPOT:
            lightSize = sizeof(GPUSpotLight);
            baseOffset += MAX_POINT_LIGHTS * sizeof(GPUPointLight);
            count = &lm->spotLightsCount;
            break;
        default: return;
    }

    if (removedIndex < *count) {
        size_t srcOffset = baseOffset + (removedIndex + 1) * lightSize;
        size_t dstOffset = baseOffset + removedIndex * lightSize;
        size_t size = (*count - removedIndex) * lightSize;

        void* temp = malloc(size);
        glGetBufferSubData(GL_UNIFORM_BUFFER, srcOffset, size, temp);
        glBufferSubData(GL_UNIFORM_BUFFER, dstOffset, size, temp);
        free(temp);
    }

    if (*count > 0) {
        size_t lastOffset = baseOffset + *count * lightSize;
        GPUPointLight empty = {0};
        glBufferSubData(GL_UNIFORM_BUFFER, lastOffset, lightSize, &empty);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void LMUpdateGPUPLight(LightManager* lm, PointLightNode* newLight)
{
  int index = LightManagerFindLightByID(lm, newLight->id, LIGHT_TYPE_POINT);
  if(index <= -1)
  {
    printf("SceneUpdateGPUPLight: no gpu light co-resonding to this light");
    return;
  }
  dynlistDeleteAt(lm->pointLights, index);
  GPUPointLight* light = GPUPointLightCreate(newLight);
  dynlistPush(lm->pointLights, &light);

}
void LMUpdateGPUSLight(LightManager* lm, SpotLightNode* newLight)
{
  int index = LightManagerFindLightByID(lm, newLight->id, LIGHT_TYPE_SPOT);
  if(index <= -1)
  {
    printf("SceneUpdateGPUSLight: no gpu light co-resonding to this light");
    return;
  }
  dynlistDeleteAt(lm->spotLights, index);
  GPUSpotLight* light = GPUSpotLightCreate(newLight);
  dynlistPush(lm->spotLights, &light);
}
void LMUpdateGPUDLight(LightManager* lm, DirectionalLightNode* newLight)
{
  free(lm->dirLight);
  GPUDirLight* light = GPUDirLightCreate(newLight);
  lm->dirLight = light;
  
}
