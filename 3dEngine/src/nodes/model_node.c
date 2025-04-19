#include "model_node.h"
#include "spatial_node.h"
#include <string.h>
#include <windows.h>


ModelNode* ModelNodeCreate(ShapeType type, const char* name, const char* modelPath)
{
  if(!modelPath && type == MODEL_CUSTOM)
  {
    printf("ModelNodeCreate: if you pass MODEL_CUSTOM modelPath should not be NULL");
    return NULL;
  }
  ModelNode* node = calloc(1, sizeof(ModelNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.visible = true;
  node->base.base.type = NODE_MODEL;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  node->shapeType = type;
  char execpath[MAX_PATH];
  if(GetModuleFileName(NULL,execpath, MAX_PATH) != 0)
  {
    char* lastSlash = strrchr(execpath, '\\');
    if(lastSlash != NULL) *lastSlash = '\0';        
    printf("Executable dir: %s\n",execpath);
  }
  char defaultModelPath[256];
  strcpy_s(defaultModelPath, 256, execpath);
  switch(type)
  {
    case MODEL_CUBE:
    strcat_s(defaultModelPath, 256,"\\resources\\models\\Cube\\Cube.obj");
    strcpy_s(node->modelPath, sizeof(node->modelPath),defaultModelPath);
    ModelInit(&node->model, node->modelPath);
    break;
    case MODEL_CYLINDER:
    strcat_s(defaultModelPath, 256,"\\resources\\models\\Cylinder\\Cylinder.obj");
    strcpy_s(node->modelPath, sizeof(node->modelPath), defaultModelPath);
    ModelInit(&node->model, node->modelPath);
    break;
    case MODEL_CAPSULE:
    strcat_s(defaultModelPath, 256,"\\resources\\models\\Capsule\\Capsule.obj");
    strcpy_s(node->modelPath, sizeof(node->modelPath),defaultModelPath);
    ModelInit(&node->model, node->modelPath);
    break;
    case MODEL_CUSTOM:
    strcpy_s(node->modelPath, sizeof(node->modelPath),modelPath);
    ModelInit(&node->model, node->modelPath);
    break;
  }
  return node;
 }


const char* ShapeTypeToStr(ShapeType type)
{
  switch(type)
  {
    case MODEL_CUSTOM: return "Mdl_custom";
    case MODEL_CUBE: return "Mdl_cube";
    case MODEL_CYLINDER: return "Mdl_cylinder";
    case MODEL_CAPSULE: return "Mdl_capsule";
  }
}
ShapeType StrToShapeType(const char* string)
{
  if(strcmp(string, "Mdl_custom") == 0) return MODEL_CUSTOM;
  if(strcmp(string, "Mdl_cube") == 0) return MODEL_CUBE;
  if(strcmp(string, "Mdl_cylinder") == 0) return MODEL_CYLINDER;
  if(strcmp(string, "Mdl_capsule") == 0) return MODEL_CAPSULE;
  printf("StrToShapeType: No such model type\n");
  return MODEL_CUSTOM; 
}
void ModelNodeToJSON(const ModelNode* node, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)node, root);
  cJSON_AddStringToObject(root, "shapetype", ShapeTypeToStr(node->shapeType));
  cJSON_AddStringToObject(root, "modelpath", node->modelPath);
}
ModelNode* ModelNodeFromJSON(const cJSON* json)
{
  const char* typeStr = cJSON_GetStringValue(cJSON_GetObjectItem(json,"shapetype"));
  ShapeType type = StrToShapeType(typeStr);
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  char* path = cJSON_GetStringValue(cJSON_GetObjectItem(json, "modelpath"));
  ModelNode* node = ModelNodeCreate(type, name, path);
  cJSON* transform = cJSON_GetObjectItem(json, "transform");
  if(transform)
  {
    cJSON* pos = cJSON_GetObjectItem(transform, "position");
    cJSON* rot = cJSON_GetObjectItem(transform, "rotation");
    cJSON* scale = cJSON_GetObjectItem(transform, "scale");
    for(int i = 0; i < 3; i++)
    {
      node->base.transform.position[i] = (float)cJSON_GetArrayItem(pos, i)->valuedouble;
      node->base.transform.rotation[i] = (float)cJSON_GetArrayItem(rot, i)->valuedouble;
      node->base.transform.scale[i] = (float)cJSON_GetArrayItem(scale, i)->valuedouble;
    }
  }
  SpatialNodeUpdateGlobalTransform((SpatialNode*)node);
  return node;
  
}

void  ModelNodeFree(ModelNode* node)
{
  if(!node)
  {
    printf("ModelNodeFree: node is invalid \n");
    return;
  }
  DeleteModel(&node->model);
  free(node);
}
