#include "light_node.h"
#include "components.h"
#include "node.h"
#include "spatial_node.h"

void PointLightCalc(PointLightNode* light)
{
  light->light.constant = 1;
  if(light->radius <= 0)
  {
    light->light.linear = 0;
    light->light.quadratic = 0;
    return;
  }
  light->light.linear = 2.0 / light->radius;
  light->light.quadratic = 1.0 / (light->radius * light->radius);
}

DirectionalLightNode* DLightCreate(const char* name, float intencity, vec3 direction)
{
  DirectionalLightNode* node = calloc(1, sizeof(DirectionalLightNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.base.type = NODE_LIGHTD;
  node->base.visible = true;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  node->intencity = intencity;
  glm_vec3_copy(direction,node->light.direction);
  glm_vec3_fill(node->light.color, 1);
  return node;  
}
PointLightNode* PLightCreate(const char* name, float intencity, float radius, int id)
{
  PointLightNode* node = calloc(1, sizeof(PointLightNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.base.type = NODE_LIGHTP;
  node->base.visible = true;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  node->intencity = intencity;
  node->radius = radius;
  node->id = id;
  PointLightCalc(node);
  glm_vec3_fill(node->light.color, 1);
  return node;
}
SpotLightNode* SLightCreate(const char* name, float intencity, vec3 direction, int id)
{
  SpotLightNode* node = calloc(1, sizeof(SpotLightNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.base.type = NODE_LIGHTS;
  node->base.visible = true;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  node->light.cutOff = 25;   
  node->light.outerCutOff = 35;   
  node->intencity = intencity;
  node->id = id;
  glm_vec3_copy(direction,node->light.direction);
  glm_vec3_fill(node->light.color, 1);
  return node;
}
DirectionalLightNode* DLightCreateDefault(const char* name)
{
  DirectionalLightNode* node = calloc(1, sizeof(DirectionalLightNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.base.type = NODE_LIGHTD;
  node->base.visible = true;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  node->intencity = 1.0;
  glm_vec3_copy((vec3){-0.45, -0.45, 0},node->light.direction);
  glm_vec3_fill(node->light.color, 1);
  return node;  
}
PointLightNode* PLightCreateDefault(const char* name)
{
  PointLightNode* node = calloc(1, sizeof(PointLightNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.base.type = NODE_LIGHTP;
  node->base.visible = true;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  node->intencity = 1.0;
  node->radius = 20;
  PointLightCalc(node);
  glm_vec3_fill(node->light.color, 1);
  return node;
}
SpotLightNode* SLightCreateDefault(const char* name)
{
  SpotLightNode* node = calloc(1, sizeof(SpotLightNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.base.type = NODE_LIGHTS;
  node->base.visible = true;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  node->light.cutOff = 25;   
  node->light.outerCutOff = 35;   
  node->intencity = 1.0;
  glm_vec3_copy((vec3){0, -1, 0},node->light.direction);
  glm_vec3_fill(node->light.color, 1);
  return node;
}
void DLightToJSON(const DirectionalLightNode* light, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)light, root);
  cJSON* color = cJSON_CreateArray();
  cJSON* dir = cJSON_CreateArray();
  for(int i = 0; i < 3; i ++)
  {
    cJSON_AddItemToArray(color, cJSON_CreateNumber(light->light.color[i]));
    cJSON_AddItemToArray(dir, cJSON_CreateNumber(light->light.direction[i]));
  }
  cJSON_AddItemToObject(root, "color", color);
  cJSON_AddItemToObject(root, "direction", dir);
  cJSON_AddNumberToObject(root, "intencity", light->intencity);
}
void PLightToJSON(const PointLightNode* light, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)light, root);
  cJSON* colorp = cJSON_CreateObject();        
  for(int i = 0; i < 3; i ++)
  {
    cJSON_AddItemToArray(colorp, cJSON_CreateNumber(light->light.color[i]));
  }
  cJSON_AddItemToObject(root, "color", colorp);
  cJSON_AddNumberToObject(root, "intencity", light->intencity);
  cJSON_AddNumberToObject(root, "radius", light->radius);
  cJSON_AddNumberToObject(root, "lightID", light->id);
}
void SLightToJSON(const SpotLightNode* light, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)light, root);
  cJSON* colorsp = cJSON_CreateObject();        
  cJSON* dirsp = cJSON_CreateObject();        
  for(int i = 0; i < 3; i ++)
  {
    cJSON_AddItemToArray(colorsp, cJSON_CreateNumber(light->light.color[i]));
    cJSON_AddItemToArray(dirsp, cJSON_CreateNumber(light->light.direction[i]));
  } 
  cJSON_AddItemToObject(root, "color", colorsp);
  cJSON_AddItemToObject(root, "direction", dirsp);
  cJSON_AddNumberToObject(root, "intencity", light->intencity);
  cJSON_AddNumberToObject(root, "innerdeg", light->light.cutOff);
  cJSON_AddNumberToObject(root, "outerdeg", light->light.outerCutOff);
  cJSON_AddNumberToObject(root, "lightID", light->id);
}
DirectionalLightNode* DLightFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  vec3 dir;
  float intencity = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json,"intencity")); 
  cJSON* direction = cJSON_GetObjectItem(json,"direction");
  dir[0] = (float)cJSON_GetArrayItem(direction, 0)->valuedouble;
  dir[1] = (float)cJSON_GetArrayItem(direction, 1)->valuedouble;
  dir[2] = (float)cJSON_GetArrayItem(direction, 2)->valuedouble;
  DirectionalLightNode* node = DLightCreate(name, intencity, dir);
  cJSON* transform = cJSON_GetObjectItem(json, "transform");
  if(transform)
  {
    cJSON* color = cJSON_GetObjectItem(json, "color"); 
    cJSON* pos = cJSON_GetObjectItem(transform, "position");
    cJSON* rot = cJSON_GetObjectItem(transform, "rotation");
    cJSON* scale = cJSON_GetObjectItem(transform, "scale");
    for(int i = 0; i < 3; i++)
    {
      node->light.color[i] = (float)cJSON_GetArrayItem(color, i)->valuedouble;
      node->base.transform.position[i] = (float)cJSON_GetArrayItem(pos, i)->valuedouble;
      node->base.transform.rotation[i] = (float)cJSON_GetArrayItem(rot, i)->valuedouble;
      node->base.transform.scale[i] = (float)cJSON_GetArrayItem(scale, i)->valuedouble;
    }
  }
  SpatialNodeUpdateGlobalTransform((SpatialNode*)node);
  return node;
}

PointLightNode* PLightFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  float intencity = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json,"intencity")); 
  float radius = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "radius"));
  int id = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "lightID"));
  PointLightNode* node = PLightCreate(name, intencity, radius, id);
  cJSON* transform = cJSON_GetObjectItem(json, "transform");
  if(transform)
  {
    cJSON* color = cJSON_GetObjectItem(json, "color"); 
    cJSON* pos = cJSON_GetObjectItem(transform, "position");
    cJSON* rot = cJSON_GetObjectItem(transform, "rotation");
    cJSON* scale = cJSON_GetObjectItem(transform, "scale");
    for(int i = 0; i < 3; i++)
    {
      node->light.color[i] = (float)cJSON_GetArrayItem(color, i)->valuedouble;
      node->base.transform.position[i] = (float)cJSON_GetArrayItem(pos, i)->valuedouble;
      node->base.transform.rotation[i] = (float)cJSON_GetArrayItem(rot, i)->valuedouble;
      node->base.transform.scale[i] = (float)cJSON_GetArrayItem(scale, i)->valuedouble;
    }
  }
  SpatialNodeUpdateGlobalTransform((SpatialNode*)node);
  return node;
}

SpotLightNode* SLightFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  float intencity = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json,"intencity"));
  float cutOff = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json,"innerdeg"));
  float outerCutOff = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json,"outerdeg"));
  int id = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "lightID"));
  vec3 dir;
  cJSON* direction = cJSON_GetObjectItem(json,"direction");
  dir[0] = (float)cJSON_GetArrayItem(direction, 0)->valuedouble;
  dir[1] = (float)cJSON_GetArrayItem(direction, 1)->valuedouble;
  dir[2] = (float)cJSON_GetArrayItem(direction, 2)->valuedouble;
  SpotLightNode* node = SLightCreate(name, intencity, dir, id);
  cJSON* transform = cJSON_GetObjectItem(json, "transform");
  if(transform)
  {
    cJSON* color = cJSON_GetObjectItem(json, "color"); 
    cJSON* pos = cJSON_GetObjectItem(transform, "position");
    cJSON* rot = cJSON_GetObjectItem(transform, "rotation");
    cJSON* scale = cJSON_GetObjectItem(transform, "scale");
    for(int i = 0; i < 3; i++)
    {
      node->light.color[i] = (float)cJSON_GetArrayItem(color, i)->valuedouble;
      node->base.transform.position[i] = (float)cJSON_GetArrayItem(pos, i)->valuedouble;
      node->base.transform.rotation[i] = (float)cJSON_GetArrayItem(rot, i)->valuedouble;
      node->base.transform.scale[i] = (float)cJSON_GetArrayItem(scale, i)->valuedouble;
    }
  }
  SpatialNodeUpdateGlobalTransform((SpatialNode*)node);
  return node;
}

void DLightFree(DirectionalLightNode* light)
{
  if(!light)
  {
    printf("DLightFree: light isn't valid\n");
    return;
  }
  free(light);
}
void PLightFree(PointLightNode* light)
{
  if(!light)
  {
    printf("PLightFree: light isn't valid\n");
    return;
  }
  free(light);
}
void SLightFree(SpotLightNode* light)
{
  if(!light)
  {
    printf("PLightFree: light isn't valid\n");
    return;
  }
  free(light);
}
