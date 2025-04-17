#include "light_node.h"
#include "spatial_node.h"


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
  cJSON_AddNumberToObject(root, "radius", light->distance);
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
  float distance = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "radius"));
  PointLightNode* node = PLightCreate(name, intencity, distance);
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
  vec3 dir;
  cJSON* direction = cJSON_GetObjectItem(json,"direction");
  dir[0] = (float)cJSON_GetArrayItem(direction, 0)->valuedouble;
  dir[1] = (float)cJSON_GetArrayItem(direction, 1)->valuedouble;
  dir[2] = (float)cJSON_GetArrayItem(direction, 2)->valuedouble;
  SpotLightNode* node = SLightCreate(name, intencity, dir);
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

