#ifndef SCENE_H
#define SCENE_H

#include "nodes/player_node.h"
#include "filehandle.h"
#include "nodes/node.h"
#include "nodes/spatial_node.h"
#include "nodes/model_node.h"
#include "nodes/camera_node.h"
#include "nodes/light_node.h"
#include "nodes/collision_node.h"
#include "gpu_light.h"

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

typedef struct
{
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;
  unsigned int texture;
} BillboardGraphics;

typedef struct
{
  Node* root;
  BillboardGraphics billboard;
  CameraNode* activeCamera;
  LightManager* lights;
  dynlist_t* renderQueue;
  bool enableLights;
} Scene;

Scene* SceneCreate(bool isLoaded);
void SceneDestroy(Scene* scene);
LightManager* SceneCreateLightManager();

void SceneInitBillboardG(Scene* scene);

void SceneDemoSetup(Scene* scene);

void SceneUpdate(Scene* scene, float delta);
void SceneRender(Scene* scene, GLuint ubo, shaderStruct* modelShader, shaderStruct* nodeShader);
void ScenePhysicsUpdateCollisions(Scene* scene);

void IsPlayerGrounded(PlayerNode* player, Scene* scene, float checkDistance); 
bool CheckSphereCollisionWithScene(vec3 sphereCenter, float sphereRadius, Scene* scene);

dynlist_t* SceneFindAllNodesOfType(Node* root, NodeType type, bool recursive);
Node* SceneFindFirstNodeOfType(Scene* scene, NodeType type);

void SceneGetAllLights(Scene* scene);
void SceneUpdateUBO(Scene* scene, GLuint ubo);
void UpdateUBOAfterRemoval(Scene* scene, GLuint ubo, LightType type, int removedIndex);
int LightManagerFindLightByID(LightManager* lm, int id, LightType type);
int LightManagerFindHigestLightID(LightManager* lm);

void SceneUpdateGPUPLight(Scene* scene, PointLightNode* newLight);
void SceneUpdateGPUSLight(Scene* scene, SpotLightNode* newLight);
void SceneUpdateGPUDLight(Scene* scene, DirectionalLightNode* newLight);
void SceneUpdateAllGPULights(Scene* scene);

void SceneRemovePLightIndex(Scene* scene, GLuint ubo, int index);
void SceneRemoveSpotIndex(Scene* scene, GLuint ubo, int index);
void SceneRemoveDLight(Scene* scene, GLuint ubo);

void SceneSave(const Scene* scene, const char* path);
Scene* SceneLoad(const char* path);

#endif // !SCENE_H
