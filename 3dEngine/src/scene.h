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
#include "light_manager.h"


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

void SceneInitBillboardG(Scene* scene);

void SceneDemoSetup(Scene* scene);

void SceneUpdate(Scene* scene, float delta);
void SceneRender(Scene* scene, GLuint ubo, shaderStruct* modelShader, shaderStruct* nodeShader);
void ScenePhysicsUpdateCollisions(Scene* scene);

void IsPlayerGrounded(PlayerNode* player, Scene* scene, float checkDistance); 
bool CheckSphereCollisionWithScene(vec3 sphereCenter, float sphereRadius, Scene* scene);

dynlist_t* SceneFindAllNodesOfType(Node* root, NodeType type, bool recursive);
Node* SceneFindFirstNodeOfType(Node* root, NodeType type);

void SceneGetAllLights(Scene* scene);
void SceneUpdateUBO(Scene* scene, GLuint ubo);

CameraNode* SceneFindActiveCamera(Scene* scene);
PlayerNode* SceneFindActivePlayer(Scene* scene);

void SceneUpdateAllGPULights(Scene* scene);


void NodeDestroyWithLightCleanup(Node* node, Scene* scene, GLuint ubo);


void SceneSave(const Scene* scene, const char* path);
Scene* SceneLoad(const char* path);

#endif // !SCENE_H
