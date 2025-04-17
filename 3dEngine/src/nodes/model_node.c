#include "model_node.h"
#include "spatial_node.h"

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

