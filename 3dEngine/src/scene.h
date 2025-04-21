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

Scene* SceneCreate();
void SceneDestroy(Scene* scene);

void SceneDemoSetup(Scene* scene);

void SceneUpdate(Scene* scene, float delta);
void SceneRender(Scene* scene, shaderStruct* modelShader);


void SceneSave(const Scene* scene, const char* path);
Scene* SceneLoad(const char* path);

#endif // !SCENE_H
