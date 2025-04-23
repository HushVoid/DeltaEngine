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


typedef struct
{
  Node* root;
  CameraNode* activeCamera;
  dynlist_t* renderQueue;
} Scene;

Scene* SceneCreate(bool isLoaded);
void SceneDestroy(Scene* scene);

void SceneDemoSetup(Scene* scene);

void SceneUpdate(Scene* scene, float delta);
void SceneRender(Scene* scene, shaderStruct* modelShader);
void ScenePhysicsUpdateCollisions(Scene* scene);

void IsPlayerGrounded(PlayerNode* player, Scene* scene, float checkDistance); 
bool CheckSphereCollisionWithScene(vec3 sphereCenter, float sphereRadius, Scene* scene);
void SceneSetupRenderQueue(Scene* scene);
dynlist_t* SceneFindAllNodesOfType(Scene* scene, NodeType type);

void SceneSave(const Scene* scene, const char* path);
Scene* SceneLoad(const char* path);

#endif // !SCENE_H
