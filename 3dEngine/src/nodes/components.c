#include "components.h"


cJSON* TransformToJSON(const Transform* t)
{
  cJSON* transform = cJSON_CreateObject();
  cJSON* position = cJSON_CreateArray();
  cJSON* rotation = cJSON_CreateArray();
  cJSON* scale = cJSON_CreateArray();
  for(int i = 0;  i < 3; i++)
  {
   cJSON_AddItemToArray(position, cJSON_CreateNumber(t->position[i])); 
   cJSON_AddItemToArray(rotation, cJSON_CreateNumber(t->rotation[i])); 
   cJSON_AddItemToArray(scale, cJSON_CreateNumber(t->scale[i])); 
  }
  cJSON_AddItemToObject(transform, "position", position);
  cJSON_AddItemToObject(transform, "rotation", rotation);
  cJSON_AddItemToObject(transform, "scale", scale);
  return transform;
}
