#include "scene.h"
#include "dynlist.h"
#include "gpu_light.h"
#include "nodes/collision_node.h"
#include "nodes/light_node.h"
#include "nodes/node.h"
#include "shader.h"

Scene* SceneCreate(bool isLoaded)
{
  Scene* scene = calloc(1, sizeof(Scene));
  scene->lights = SceneCreateLightManager();
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

LightManager* SceneCreateLightManager()
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
void SceneDestroy(Scene* scene)
{
  NodeDestroy(scene->root);
  dynlistFree(scene->renderQueue);
  free(scene);
}

void SceneUpdateUBO(Scene* scene, GLuint ubo)
{
  if(!scene || !ubo)
  {
    printf("SceneUpdateUBO: Davay poka");
    return;
  }

  if(scene->lights->dirLight)
  {
    DirLight_UploadToUBO(ubo,scene->lights->dirLight);
  }
  for(unsigned int i = 0; i < scene->lights->pointLightsCount; i++)
  {
    GPUPointLight* light = *(GPUPointLight**)dynlistAt(scene->lights->pointLights, i); 
    PointLight_UploadToUBO(ubo, light, i);
  }
  for(unsigned int i = 0; i < scene->lights->spotLightsCount; i++)
  {
    GPUSpotLight* light = *(GPUSpotLight**)dynlistAt(scene->lights->spotLights, i);
    SpotLight_UploadToUBO(ubo, light, i);
  }
  scene->lights->lightsDirty = false;
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
  dynlist_t * colliders = SceneFindAllNodesOfType(scene->root, NODE_COLLISION, true);
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
void SceneRender(Scene* scene, GLuint ubo, shaderStruct* modelShader)
{
  for(int i = 0; i < scene->renderQueue->size; i++)
  {
    Node* renderNode = *(Node**)dynlistAt(scene->renderQueue, i);
    if(renderNode->type == NODE_MODEL)
      ModelNodeRender((ModelNode*)renderNode,modelShader);
    UseShader(modelShader);
    SetShaderFloat3(modelShader, "viewPos", scene->activeCamera->base.transform.position);
    SetShaderInt(modelShader, "activePointLights", scene->lights->pointLightsCount);
    SetShaderInt(modelShader, "activeSpotLights", scene->lights->spotLightsCount);
    if(scene->lights->lightsDirty)
    {
      SceneUpdateUBO(scene, ubo);
    }
  }
}

//VERY memory leaky free the returned list please
dynlist_t* SceneFindAllNodesOfType(Node* root, NodeType type, bool recursive)
{
  dynlist_t* nodes = dynlistInit(sizeof(Node*), 4);
  if(!recursive)
  {
    for(int i = 0; i < root->children->size; i++)
    {
      Node ** node = (Node**)dynlistAt(root->children, i);
      if(node && (*node)->type == type)
      {
        dynlistPush(nodes, node);
      }
    }
  }
  else 
  {   
    if (root->type == type) 
    {
        dynlistPush(nodes, &root);
    }
      if (root->children && root->children->size > 0) 
      {
        for (int i = 0; i < root->children->size; i++) 
        {
          Node** child = (Node**)dynlistAt(root->children, i);
          if (*child) 
          {
              dynlist_t* childNodes = SceneFindAllNodesOfType(*child, type, true);
              
              for (int j = 0; j < childNodes->size; j++) 
              {
                  Node** foundNode = (Node**)dynlistAt(childNodes, j);
                  dynlistPush(nodes, foundNode);
              }
              dynlistFree(childNodes);
          }
        }
      }
  }
  return nodes;
}
Node* SceneFindFirstNodeOfType(Scene* scene, NodeType type)
{
  for(int i = 0; i < scene->root->children->size; i++)
  {
    Node * node = *(Node**)dynlistAt(scene->root->children, i);
    if(node && node->type == type)
    {
      return node; 
    }
  }
  printf("SceneFindFirstNodeOfType: No nodes with type %s", NodeT2Str(type));
  return NULL;
}
void ScenePhysicsUpdateCollisions(Scene* scene)
{
  dynlist_t* colliders = SceneFindAllNodesOfType(scene->root, NODE_COLLISION, true);
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
    return NULL;
  }
  cJSON* json = cJSON_Parse(json_str);
  Scene* scene = SceneCreate(true);
  scene->root = NodeFromJSON(json);
  scene->activeCamera = (CameraNode*)NodeFindChild(scene->root, "MAINCAM", false);
  SceneSetupRenderQueue(scene);
  SceneGetAllLights(scene);
  LightManagerFindHigestLightID(scene->lights);
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
void SceneGetAllLights(Scene* scene)
{
  DirectionalLightNode* dirL = (DirectionalLightNode*)SceneFindFirstNodeOfType(scene, NODE_LIGHTD); 
  dynlist_t* pointL = SceneFindAllNodesOfType(scene->root, NODE_LIGHTP, true);  
  dynlist_t* spotL = SceneFindAllNodesOfType(scene->root, NODE_LIGHTS, true);
  if(dirL)
  {
    GPUDirLight* dirLS = GPUDirLightCreate(dirL);
    scene->lights->dirLight = dirLS;
  }
  for(int i = 0; i < pointL->size; i++)
  {
    PointLightNode* light = (PointLightNode*)*(Node**)dynlistAt(pointL, i);
    GPUPointLight* shader_light = GPUPointLightCreate(light);
    dynlistPush(scene->lights->pointLights, &shader_light);
  }
  scene->lights->pointLightsCount = pointL->size; 
  for(int i = 0; i < spotL->size; i++)
  {
    SpotLightNode* light = (SpotLightNode*)*(Node**)dynlistAt(pointL, i);
    GPUSpotLight* shader_light = GPUSpotLightCreate(light);
    dynlistPush(scene->lights->spotLights, &shader_light);
  }
  scene->lights->pointLightsCount = spotL->size;
  dynlistFree(pointL);
  dynlistFree(spotL);
}

void SceneRemovePLightIndex(Scene* scene, GLuint ubo, int index)
{
  dynlistDeleteAt(scene->lights->pointLights, index);
  scene->lights->pointLightsCount--;
  UpdateUBOAfterRemoval(scene, ubo, LIGHT_TYPE_POINT, index);
}
void SceneRemoveSpotIndex(Scene* scene, GLuint ubo, int index)
{
  dynlistDeleteAt(scene->lights->spotLights, index);
  scene->lights->spotLightsCount--;
  UpdateUBOAfterRemoval(scene, ubo, LIGHT_TYPE_SPOT, index);
  
}
void SceneRemoveDLight(Scene* scene, GLuint ubo)
{
 free(scene->lights->dirLight); 
 scene->lights->dirLight = NULL;
 UpdateUBOAfterRemoval(scene, ubo, LIGHT_TYPE_DIR, 0);
}
void SceneUpdateGPUPLight(Scene* scene, PointLightNode* newLight)
{
  int index = LightManagerFindLightByID(scene->lights, newLight->id, LIGHT_TYPE_POINT);
  if(index <= -1)
  {
    printf("SceneUpdateGPUPLight: no gpu light co-resonding to this light");
    return;
  }
  dynlistDeleteAt(scene->lights->pointLights, index);
  GPUPointLight* light = GPUPointLightCreate(newLight);
  dynlistPush(scene->lights->pointLights, &light);
}
void SceneUpdateGPUSLight(Scene* scene, SpotLightNode* newLight)
{
  int index = LightManagerFindLightByID(scene->lights, newLight->id, LIGHT_TYPE_SPOT);
  if(index <= -1)
  {
    printf("SceneUpdateGPUSLight: no gpu light co-resonding to this light");
    return;
  }
  dynlistDeleteAt(scene->lights->spotLights, index);
  GPUSpotLight* light = GPUSpotLightCreate(newLight);
  dynlistPush(scene->lights->spotLights, &light);
}
void SceneUpdateGPUDLight(Scene* scene, DirectionalLightNode* newLight)
{
  free(scene->lights->dirLight);
  GPUDirLight* light = GPUDirLightCreate(newLight);
  scene->lights->dirLight = light;
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
      return index;
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
}

void UpdateUBOAfterRemoval(Scene* scene, GLuint ubo, LightType type, int removedIndex) 
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
            count = &scene->lights->pointLightsCount;
            break;
        case LIGHT_TYPE_SPOT:
            lightSize = sizeof(GPUSpotLight);
            baseOffset += MAX_POINT_LIGHTS * sizeof(GPUPointLight);
            count = &scene->lights->spotLightsCount;
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
