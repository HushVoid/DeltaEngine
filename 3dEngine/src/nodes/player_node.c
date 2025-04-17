#include "player_node.h"
#include "spatial_node.h"


void PlayerNodeToJSON(const PlayerNode* node, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)node, root);
  cJSON_AddBoolToObject(root, "gravityAffected", node->gravityAffected);
  cJSON_AddNumberToObject(root, "speed", node->speed);
}
PlayerNode* PlayerNodeFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  float speed = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "speed"));
  bool isGravity = cJSON_IsTrue(cJSON_GetObjectItem(json,"object"));
  PlayerNode* node = PlayerNodeCreate(speed, name, isGravity);
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
