#include "scene.h"

Scene* SceneCreate()
{
  Scene* scene = calloc(1, sizeof(Scene));
  scene->root = NodeCreate("root");
  scene->activeCamera = CameraNodeCreate("MAINCAM", 60, (vec3){0,1,0}, 0.125, 100, 16.0/9.0);
  NodeAddChild(scene->root, (Node*)scene->activeCamera);
  scene->renderQueue = dynlistInit(sizeof(Node*), 4);
  return scene;
}

void SceneDestroy(Scene* scene)
{
  NodeDestroy(scene->root);
  dynlistFree(scene->renderQueue);
  free(scene);
}

void SceneDemoSetup(Scene* scene)
{
  ModelNode* cube = ModelNodeCreate(MODEL_CUBE, "cube", NULL); 
  ModelNode* cylinder = ModelNodeCreate(MODEL_CYLINDER, "cylinder", NULL); 
  ModelNode* capsule = ModelNodeCreate(MODEL_CAPSULE, "cube", NULL);
  ModelNode* backpack = ModelNodeCreate(MODEL_CUSTOM, "cube", "E:\\projects\\deltaengine\\3dengine\\resources\\models\\backpack\\backpack.obj");
  Node* cubeNode_t = (Node*)cube;
  Node* cylinderNode_t = (Node*)cylinder;
  Node* capsuleNode_t= (Node*)capsule;
  Node* backpackNode_t = (Node*) backpack;
  dynlistPush(scene->renderQueue, &cubeNode_t);
  dynlistPush(scene->renderQueue, &cylinderNode_t);
  dynlistPush(scene->renderQueue, &capsuleNode_t);
  dynlistPush(scene->renderQueue, &backpackNode_t);
  NodeAddChild(scene->root, cubeNode_t);
  NodeAddChild(scene->root, cylinderNode_t);
  NodeAddChild(scene->root, capsuleNode_t);
  NodeAddChild(scene->root, backpackNode_t);
  SpatialNodeSetPos((SpatialNode*)cube , (vec3){3,0,0});
  SpatialNodeSetPos((SpatialNode*)cylinder , (vec3){-3,0,0});
  SpatialNodeSetPos((SpatialNode*)cube , (vec3){0,0,3});
  SpatialNodeSetPos((SpatialNode*)backpack , (vec3){0,0,-3});
}

void SceneRender(Scene* scene, shaderStruct* modelShader)
{
  for(int i = 0; i < scene->renderQueue->size; i++)
  {
    Node* renderNode = *(Node**)dynlistAt(scene->renderQueue, i);
    if(renderNode->type == NODE_MODEL)
      ModelNodeRender((ModelNode*)renderNode,modelShader);
  }
}
void SceneSave(const Scene* scene, const char* path)
{
  char* json_str = NodeToJSON(scene->root);
  WriteToFile(path, json_str);
  free(json_str);
}
Scene* SceneLoad(const char* path)
{
  char* json_str = ReadFromFile(path);
  if(!json_str)
  {
    printf("SceneLoad: No such scene file");
  }
  cJSON* json = cJSON_Parse(json_str);
  Scene* scene = SceneCreate();
  scene->root = NodeFromJSON(json);
  scene->activeCamera = (CameraNode*)NodeFindChild(scene->root, "MAINCAM", false);
  free(json_str);
  cJSON_Delete(json);
  return scene;
}
