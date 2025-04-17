#include "camera_node.h"
#include "spatial_node.h"

void nodeeraNodeToJSON(const CameraNode* node, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)node, root);
  cJSON_AddNumberToObject(root, "fov", node->fov); 
  cJSON_AddNumberToObject(root, "nearplane", node->nearPlane); 
  cJSON_AddNumberToObject(root, "farplane", node->farPlane); 
  cJSON_AddNumberToObject(root, "aspect", node->aspect); 
}
CameraNode* CameraNodeFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  float near = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "nearplane"));  
  float far = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "farplane"));  
  float aspect = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "aspect"));
  CameraNode* node = CameraNodeCreate(name, (vec3){0, 1, 0}, near, far, aspect);
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
