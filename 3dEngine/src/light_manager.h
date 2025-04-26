#ifndef  LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include "gpu_light.h"
#include "nodes/light_node.h"

typedef struct 
{
  GPUDirLight* dirLight;
  dynlist_t* pointLights; // dynlist of GPUSpotLight**
  dynlist_t* spotLights; //dynlist of GPUPointLight**
  int pointLightsCount;
  int spotLightsCount;
  int nextLightid;
  bool lightsDirty;
} LightManager;

LightManager* LightManagerCreate();
void UpdateUBOAfterRemoval(LightManager* lm, GLuint ubo, LightType type, int removedIndex);


void LMUpdateGPUPLight(LightManager* lm, PointLightNode* newLight);
void LMUpdateGPUSLight(LightManager* lm, SpotLightNode* newLight);
void LMUpdateGPUDLight(LightManager* lm, DirectionalLightNode* newLight );

void LMRemovePLightIndex(LightManager* lm, GLuint ubo, int index);
void LMRemoveSpotIndex(LightManager* lm, GLuint ubo, int index);
void LMRemoveDLight(LightManager* lm, GLuint ubo);

int LightManagerFindLightByID(LightManager* lm, int id, LightType type);
int LightManagerFindHigestLightID(LightManager* lm);
#endif // ! LIGHT_MANAGER_H
