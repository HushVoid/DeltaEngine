#include "spatial_node.h"

void SpatialNodeToJSON(const SpatialNode* node, cJSON* root)
{
  cJSON* transform = TransformToJSON(&node->transform);
  cJSON_AddItemToObject(root, "transform", transform);
  cJSON_AddBoolToObject(root, "visible", node->visible);
}

SpatialNode* SpatialNodeFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  SpatialNode* node = SpatialNodeCreate(name);
  cJSON* transform = cJSON_GetObjectItem(json, "transform");
  if(transform)
  {
    cJSON* pos = cJSON_GetObjectItem(transform, "position");
    cJSON* rot = cJSON_GetObjectItem(transform, "rotation");
    cJSON* scale = cJSON_GetObjectItem(transform, "scale");
    for(int i = 0; i < 3; i++)
    {
      node->transform.position[i] = (float)cJSON_GetArrayItem(pos, i)->valuedouble;
      node->transform.rotation[i] = (float)cJSON_GetArrayItem(rot, i)->valuedouble;
      node->transform.scale[i] = (float)cJSON_GetArrayItem(scale, i)->valuedouble;
    }
  }
  const cJSON* visible = cJSON_GetObjectItem(json, "visible");
  if (visible)
  {
    node->visible = cJSON_IsTrue(visible);
  }
  SpatialNodeUpdateGlobalTransform(node); 
  return node;
   
}
