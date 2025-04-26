#include "components.h"

void TransformDefaultInit(Transform *t)
{
  glm_vec3_fill(t->position, 0);
  glm_vec3_fill(t->rotation, 0);
  glm_vec3_fill(t->scale, 1);
}

void TransformCopyTo(Transform *src, Transform *dest)
{
  glm_vec3_copy(src->scale, dest->scale);
  glm_vec3_copy(src->position, dest->position);
  glm_vec3_copy(src->rotation, dest->rotation);
}
void TransformGetForward(const Transform* t, vec3 dest)
{
  float pitch = DEG2RAD(t->rotation[0]);
  float yaw = DEG2RAD(t->rotation[1]);
  vec3 forward = {
    -sinf(yaw) * cosf(pitch),
    sinf(pitch),
    -cosf(yaw) * cosf(pitch)
  };
  glm_vec3_normalize(forward);
  glm_vec3_copy(forward, dest);
}
void TransformGetForwardNoPitch(const Transform* t, vec3 dest)
{
  float yaw = DEG2RAD(t->rotation[1]);
  vec3 forward = {
    -sinf(yaw),
    0,
    -cosf(yaw) 
  };
  glm_vec3_normalize(forward);
  glm_vec3_copy(forward, dest);
}
void TransformGetRightVec(const Transform *t,vec3 worldUp, vec3 dest)
{
  vec3 right;
  vec3 forward;
  TransformGetForward(t, forward);
  glm_vec3_cross(forward, worldUp, right);
  glm_vec3_normalize(right);
  glm_vec3_copy(right, dest);
}
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
