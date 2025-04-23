#include "scene.h"
#include "dynlist.h"
#include "nodes/collision_node.h"
#include "nodes/node.h"

Scene* SceneCreate(bool isLoaded)
{
  Scene* scene = calloc(1, sizeof(Scene));
  if(!isLoaded) 
  {
    scene->root = NodeCreate("root");
    scene->activeCamera = CameraNodeCreate("MAINCAM", 60, (vec3){0,1,0}, 0.125, 100, 16.0/9.0);
    NodeAddChild(scene->root, (Node*)scene->activeCamera); 
  } 
  else 
  {
   scene->root = NULL;
   scene->activeCamera = NULL;
  }
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
  ModelNode* capsule = ModelNodeCreate(MODEL_CAPSULE, "capsule", NULL);
  ModelNode* backpack = ModelNodeCreate(MODEL_CUSTOM, "backpack", "E:\\projects\\deltaengine\\3dengine\\resources\\models\\backpack\\backpack.obj");
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
bool CheckSphereCollisionWithScene(vec3 sphereCenter, float sphereRadius, Scene* scene) 
{
 if(!scene)
  {
    printf("CheckSphereCollisionWithScene: scene is invalid\n");
    return false;
  }
  dynlist_t * colliders = SceneFindAllNodesOfType(scene, NODE_COLLISION);
  for(int i = 0; i < colliders->size; i++)
  {
    ColliderNode* col = (ColliderNode*)*(Node**)dynlistAt(colliders, i);
      
    vec3 colMin, colMax;
    ColliderNodeGetWorldAABB(col, colMin, colMax);
      
    if (CheckSphereCollision(sphereCenter, sphereRadius, colMin, colMax)) 
    {
      dynlistFree(colliders);
      return true;
    }
  }
  dynlistFree(colliders);
  return false;
}
void IsPlayerGrounded(PlayerNode* player, Scene* scene, float checkDistance) 
{
    if (!player || !scene) return;
    
    vec3 checkPoint;
    glm_vec3_copy(player->base.transform.position, checkPoint);
    checkPoint[1] -= checkDistance; 
    
    bool isColiding =  CheckSphereCollisionWithScene(
        checkPoint,
        0.3f, 
        scene
    );
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

//VERY memory leaky free the retured list please
dynlist_t* SceneFindAllNodesOfType(Scene* scene, NodeType type)
{
  dynlist_t* nodes = dynlistInit(sizeof(Node*), 4);
  for(int i = 0; i < scene->root->children->size; i++)
  {
    Node ** node = (Node**)dynlistAt(scene->root->children, i);
    if(node && (*node)->type == type)
    {
      dynlistPush(nodes, node);
    }
  }
  return nodes;
}
void ScenePhysicsUpdateCollisions(Scene* scene)
{
  dynlist_t* colliders = SceneFindAllNodesOfType(scene, NODE_COLLISION);
  for(int i = 0; i < colliders->size; i++ )
  {
    ColliderNode* a  = (ColliderNode*)*(Node**)dynlistAt(colliders, i);
    for(int j = i + 1; j < colliders->size; j++)
    {
      ColliderNode* b = (ColliderNode*)*(Node**)dynlistAt(colliders, j);

      bool isColiding = ColliderNodeCheckCollision(a, b);
      ColliderNodeHandleStateChange(a, b, isColiding);
      if(isColiding && !a->isTrigger && !b->isTrigger)
      {
        ColliderNodeResolveCollision(a, b);
      }
    }
  }
  dynlistFree(colliders);
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
  Scene* scene = SceneCreate(true);
  scene->root = NodeFromJSON(json);
  scene->activeCamera = (CameraNode*)NodeFindChild(scene->root, "MAINCAM", false);
  SceneSetupRenderQueue(scene);
  free(json_str);
  cJSON_Delete(json);
  return scene;
}
void SceneSetupRenderQueue(Scene* scene)
{
 if(scene->root->children->size == 0)
  {
    printf("SceneSetupRenderQueue: root is empty");
    return;
  }
  for(int i = 0; i < scene->root->children->size; i++)
  {
    Node* node = *(Node**)dynlistAt(scene->root->children, i);
    if(node->type == NODE_MODEL)
    {
      dynlistPush(scene->renderQueue, &node);
    }
  }
  
}
