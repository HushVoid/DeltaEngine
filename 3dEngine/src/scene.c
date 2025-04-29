#include "scene.h"
#include "dynlist.h"
#include "gpu_light.h"
#include "nodes/collision_node.h"
#include "nodes/light_node.h"
#include "nodes/node.h"
#include "nodes/spatial_node.h"
#include "shader.h"
#include <string.h>
#include <windows.h>
#include "include/STBI/stb_image.h"

Scene* SceneCreate(bool isLoaded)
{
  Scene* scene = calloc(1, sizeof(Scene));
  scene->lights = LightManagerCreate();
  SceneInitBillboardG(scene);
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

void SceneInitBillboardG(Scene* scene)
{
  if (!scene) {
      printf("Error: Scene is NULL in SceneInitBillboardG\n");
      return;
  }
  char execpath[MAX_PATH];
  if(GetModuleFileName(NULL,execpath, MAX_PATH) != 0)
  {
    char* lastSlash = strrchr(execpath, '\\');
    if(lastSlash != NULL) *lastSlash = '\0';        
  }
  float billboardVertices[] = {
      // positions   // texCoords
      -0.5f, -0.5f,  0.0f, 0.0f,
       0.5f, -0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.0f, 1.0f
  };

  unsigned int billboardIndices[] = {
      0, 1, 2,
      2, 3, 0
  };

  glGenVertexArrays(1, &scene->billboard.VAO);
  glGenBuffers(1, &scene->billboard.VBO);
  glGenBuffers(1, &scene->billboard.EBO);

  glBindVertexArray(scene->billboard.VAO);

  glBindBuffer(GL_ARRAY_BUFFER, scene->billboard.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(billboardVertices), billboardVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene->billboard.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(billboardIndices), billboardIndices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);


  char loadPath[256];
  strcpy_s(loadPath, sizeof(loadPath), execpath);
  strcat_s(loadPath,sizeof(loadPath), "\\textures\\node.png");
  int width, height, nrChannels;
  unsigned char *data = stbi_load(loadPath, &width, &height, &nrChannels, 0); 
  if (!data) 
  {
    printf("Failed to load texture: %s\n", loadPath);
    return;
  }
  glGenTextures(1, &scene->billboard.texture);
  glBindTexture(GL_TEXTURE_2D, scene->billboard.texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

}
void SceneDestroy(Scene* scene)
{
  NodeDestroy(scene->root);
  dynlistFree(scene->renderQueue);
  dynlistFree(scene->lights->pointLights);
  dynlistFree(scene->lights->spotLights);
  free(scene->lights);
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
    SpatialGetGlobalPos((SpatialNode*)player, checkPoint);
    checkPoint[1] -= checkDistance; 
    
    bool isColiding =  CheckSphereCollisionWithScene(
        checkPoint,
        0.3f, 
        scene
    );
    player->isGrounded = isColiding;
}
void SceneRender(Scene* scene, GLuint ubo, shaderStruct* modelShader, shaderStruct* nodeShader)
{  


  for(int i = 0; i < scene->renderQueue->size; i++)
  {
    Node* renderNode = *(Node**)dynlistAt(scene->renderQueue, i);
    if(renderNode->type == NODE_MODEL)
    {
      ModelNodeRender((ModelNode*)renderNode,modelShader);
      UseShader(modelShader);
      SetShaderBool(modelShader, "lightEnabled", scene->enableLights);
      if(scene->enableLights)
      {
        SetShaderInt(modelShader, "activePointLights", scene->lights->pointLightsCount);
        SetShaderInt(modelShader, "activeSpotLights", scene->lights->spotLightsCount);
        if(scene->lights->lightsDirty)
        {
          SceneUpdateAllGPULights(scene);
          SceneUpdateUBO(scene, ubo);
        }
      }
      
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
Node* SceneFindFirstNodeOfType(Node* root, NodeType type)
{

  for(int i = 0; i < root->children->size; i++)
  {
    Node* child = *(Node**)dynlistAt(root->children, i);
    Node* found = SceneFindFirstNodeOfType(child, type);
    if(found) return found;
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
  if(!json)
  {
    printf("SceneLoad: corupted scene file");
    return NULL;
  }
  Scene* scene = SceneCreate(true);
  scene->root = NodeFromJSON(json, scene->renderQueue);
  scene->activeCamera = (CameraNode*)NodeFindChild(scene->root, "MAINCAM", false);
  SceneGetAllLights(scene);
  printf("proccessed scene\n");
  LightManagerFindHigestLightID(scene->lights);
  SceneInitBillboardG(scene);
  scene->lights->lightsDirty = true;
  free(json_str);
  cJSON_Delete(json);
  return scene;
}

void SceneGetAllLights(Scene* scene)
{
  dynlist_t* dirL = SceneFindAllNodesOfType(scene->root, NODE_LIGHTD, true); 
  dynlist_t* pointL = SceneFindAllNodesOfType(scene->root, NODE_LIGHTP, true);  
  dynlist_t* spotL = SceneFindAllNodesOfType(scene->root, NODE_LIGHTS, true);
  if(dirL && dirL->size >0)
  {
    DirectionalLightNode* light = (DirectionalLightNode*)*(Node**)dynlistAt(dirL, 0);
    GPUDirLight* dirLS = GPUDirLightCreate(light);
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
    SpotLightNode* light = (SpotLightNode*)*(Node**)dynlistAt(spotL, i);
    GPUSpotLight* shader_light = GPUSpotLightCreate(light);
    dynlistPush(scene->lights->spotLights, &shader_light);
  }
  printf("s kaifom\n");
  scene->lights->spotLightsCount = spotL->size;
  dynlistFree(pointL);
  dynlistFree(spotL);
}



void SceneUpdateAllGPULights(Scene* scene)
{ 
  dynlist_t* pointL = SceneFindAllNodesOfType(scene->root, NODE_LIGHTP, true);  
  dynlist_t* spotL = SceneFindAllNodesOfType(scene->root, NODE_LIGHTS, true);
  for(int i = 0; i < pointL->size; i++)
  {
    PointLightNode* light = (PointLightNode*)*(Node**)dynlistAt(pointL, i);
    LMUpdateGPUPLight(scene->lights, light);    
  }
  for(int i = 0; i < spotL->size; i++)
  {
    SpotLightNode* light = (SpotLightNode*)*(Node**)dynlistAt(spotL, i);
    LMUpdateGPUSLight(scene->lights, light);
  }
  dynlistFree(pointL);
  dynlistFree(spotL);
  
}

CameraNode* SceneFindActiveCamera(Scene* scene)
{
  dynlist_t* cameras = SceneFindAllNodesOfType(scene->root, NODE_CAMERA, true);
  for(int i = 0; i < cameras->size; i++)
  {
    CameraNode* camera = (CameraNode*)*(Node**)dynlistAt(cameras, i);
    if(camera->isActive)
    {
      return camera;
    }
  }
  printf("No active cameras\n");
  dynlistFree(cameras);
  return NULL; 
}

PlayerNode* SceneFindActivePlayer(Scene* scene)
{
  dynlist_t* players = SceneFindAllNodesOfType(scene->root, NODE_PLAYER, true);
  for(int i = 0; i < players->size; i++)
  {
    PlayerNode* player = (PlayerNode*)*(Node**)dynlistAt(players, i);
    if(player->isActive)
    {
      return player;
    }
  }
  printf("No active players\n");
  dynlistFree(players);
  return NULL; 
  
}

void NodeDestroyWithLightCleanup(Node* node, Scene* scene, GLuint ubo)
{
    if(!node) return;
    
    if(node->children)
    {
        for(int i = 0; i < node->children->size; i++)
        {
            Node* child = *(Node**)dynlistAt(node->children, i);
            NodeDestroyWithLightCleanup(child, scene, ubo);
        }
        dynlistFree(node->children);
    }
    
    switch(node->type)
    {
        case NODE_LIGHTD:
            LMRemoveDLight(scene->lights, ubo);
            scene->lights->lightsDirty = true;
            DLightFree((DirectionalLightNode*)node);
            return;
            
        case NODE_LIGHTP:
        {
            PointLightNode* light = (PointLightNode*)node;
            int index = LightManagerFindLightByID(scene->lights, light->id, LIGHT_TYPE_POINT);
            LMRemovePLightIndex(scene->lights, ubo, index);
            scene->lights->lightsDirty = true;
            PLightFree(light);
            return;
        }
            
        case NODE_LIGHTS:
        {
            SpotLightNode* light = (SpotLightNode*)node;
            int index = LightManagerFindLightByID(scene->lights, light->id, LIGHT_TYPE_SPOT);
            LMRemoveSpotIndex(scene->lights, ubo, index);
            scene->lights->lightsDirty = true;
            SLightFree(light);
            return;
        }
        case NODE_SPATIAL:
            SpatialNodeFree((SpatialNode*)node);
            return;
        case NODE_CAMERA:
            CameraNodeFree((CameraNode*)node);
            return;
        case NODE_MODEL:
          int index = 0;
          for(int i = 0; i < scene->renderQueue->size; i++)
          {
            Node* rendered = *(Node**)dynlistAt(scene->renderQueue, i++);
            if(node == rendered)
            {
              index = i;
            }
          }
          dynlistDeleteAt(scene->renderQueue, index);
          ModelNodeFree((ModelNode*)node);
          return;
        case NODE_PLAYER:
          PlayerNodeFree((PlayerNode*) node);
          return;
        case NODE_COLLISION:
          ColliderNodeFree((ColliderNode*) node);
          return;
            
        default:
            free(node);
            return;
    }
}
