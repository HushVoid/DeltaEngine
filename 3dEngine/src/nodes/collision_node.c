#include "collision_node.h"
#include <float.h>


ColliderNode* ColliderNodeCreate(const char* name, vec3 min, vec3 max, bool isTrigger)
{
  ColliderNode* node = calloc(1, sizeof(ColliderNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.visible = true;
  node->base.base.type = NODE_COLLISION;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  glm_vec3_copy(min, node->min);
  glm_vec3_copy(max, node->max);
  node->isTrigger = isTrigger;
  return node;
}
ColliderNode* ColliderNodeCreateDefault(const char* name)
{
  ColliderNode* node = calloc(1, sizeof(ColliderNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.visible = true;
  node->base.base.type = NODE_COLLISION;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  glm_vec3_fill(node->min, -1);
  glm_vec3_fill(node->max, 1);
  node->isTrigger = false;
  return node;
}
void ColliderNodeGetWorldAABB(const ColliderNode* collider, vec3 outMin, vec3 outMax)
{
  vec3 corners[8] = 
  {
    // Bottom face (z = min.z)
    {collider->min[0], collider->min[1], collider->min[2]},  // 0: Left-front-bottom
    {collider->max[0], collider->min[1], collider->min[2]},  // 1: Right-front-bottom
    {collider->max[0], collider->max[1], collider->min[2]},  // 2: Right-back-bottom
    {collider->min[0], collider->max[1], collider->min[2]},  // 3: Left-back-bottom

    // Top face (z = max.z)
    {collider->min[0], collider->min[1], collider->max[2]},  // 4: Left-front-top
    {collider->max[0], collider->min[1], collider->max[2]},  // 5: Right-front-top
    {collider->max[0], collider->max[1], collider->max[2]},  // 6: Right-back-top
    {collider->min[0], collider->max[1], collider->max[2]}   // 7: Left-back-top
    // 
  };
  glm_vec3_fill(outMax, FLT_MIN);
  glm_vec3_fill(outMin, FLT_MAX);
  for(int i = 0; i < 8; i++)
  {
    mat4 globalMatrix;
    vec3 worldPos;
    glm_mat4_mulv3(collider->base.globalTransformMatrix, corners[i], 1.0, worldPos);
    outMin[0] = fminf(outMin[0], worldPos[0]);
    outMin[1] = fminf(outMin[1], worldPos[1]);
    outMin[2] = fminf(outMin[2], worldPos[2]);
    outMax[0] = fmaxf(outMax[0], worldPos[0]);
    outMin[1] = fmaxf(outMax[1], worldPos[1]);
    outMin[2] = fmaxf(outMax[2], worldPos[2]);
  }
}
bool ColliderNodeCheckCollision(const ColliderNode* a, const ColliderNode* b)
{
  vec3 aMin, aMax, bMin, bMax;
  ColliderNodeGetWorldAABB(a, aMin, aMax);
  ColliderNodeGetWorldAABB(b, bMin, bMax);
  return (aMin[0] <= bMax[0] && aMax[0] >= bMin[0]) &&
         (aMin[1] <= bMax[1] && aMax[1] >= bMin[1]) &&
         (aMin[2] <= bMax[2] && aMax[2] >= bMin[2]);
}

void ColliderNodeToJSON(const ColliderNode* node, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)node, root);
  cJSON* min = cJSON_CreateArray();
  cJSON* max = cJSON_CreateArray();
  for(int i = 0; i < 3; i++)
  {
    cJSON_AddItemToArray(min, cJSON_CreateNumber(node->min[i]));
    cJSON_AddItemToArray(max, cJSON_CreateNumber(node->max[i]));
  }
  cJSON_AddItemToObject(root, "minAABB", min);
  cJSON_AddItemToObject(root, "maxAABB", max);
  cJSON_AddBoolToObject(root, "isTrigger", node->isTrigger);
}
ColliderNode* ColliderNodeFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json, "name"));
  vec3 min;
  vec3 max;
  cJSON* minAABB = cJSON_GetObjectItem(json, "minAABB");
  cJSON* maxAABB = cJSON_GetObjectItem(json, "maxAABB");
  for(int i = 0; i < 3; i++)
  {
   min[i] = (float)cJSON_GetArrayItem(minAABB, i)->valuedouble; 
   max[i] = (float)cJSON_GetArrayItem(maxAABB, i)->valuedouble; 
  }
  bool isTrigger = cJSON_IsTrue(cJSON_GetObjectItem(json, "isTrigger"));
  ColliderNode* node = ColliderNodeCreate(name, min, max, isTrigger);
  return node;
}

void ColliderNodeFree(ColliderNode* node)
{
  if(!node)
  {
    printf("CollisionNodeFree: node isn't valid\n");
  }
  free(node);
}
