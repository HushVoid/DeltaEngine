#include "collision_node.h"
#include "spatial_node.h"
#include <float.h>


ColliderNode* ColliderNodeCreate(const char* name, vec3 min, vec3 max, bool isTrigger, bool isStatic)
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
  node->isStatic = isStatic;
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
  node->isStatic = true;
  return node;
}
void ColliderNodeGetWorldAABB(const ColliderNode* collider, vec3 outMin, vec3 outMax)
{
    vec3 corners[8] = 
    {
        {collider->min[0], collider->min[1], collider->min[2]},
        {collider->max[0], collider->min[1], collider->min[2]},
        {collider->max[0], collider->max[1], collider->min[2]},
        {collider->min[0], collider->max[1], collider->min[2]},
        {collider->min[0], collider->min[1], collider->max[2]},
        {collider->max[0], collider->min[1], collider->max[2]},
        {collider->max[0], collider->max[1], collider->max[2]},
        {collider->min[0], collider->max[1], collider->max[2]}
    };

    glm_vec3_fill(outMin, FLT_MAX);
    glm_vec3_fill(outMax, -FLT_MAX);

    for(int i = 0; i < 8; i++) 
   {
        vec4 worldPos;
        glm_mat4_mulv3(collider->base.globalTransformMatrix, corners[i], 1.0f, worldPos);
        
        outMin[0] = fminf(outMin[0], worldPos[0]);
        outMin[1] = fminf(outMin[1], worldPos[1]);
        outMin[2] = fminf(outMin[2], worldPos[2]);
        
        outMax[0] = fmaxf(outMax[0], worldPos[0]);
        outMax[1] = fmaxf(outMax[1], worldPos[1]);
        outMax[2] = fmaxf(outMax[2], worldPos[2]);
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

void ColliderNodeHandleStateChange(ColliderNode* a, ColliderNode* b, bool state)
{
  a->isColliding = state;
  b->isColliding = state;
}
void ColliderNodeResolveCollision(ColliderNode* a, ColliderNode*b)
{
    printf("assalamu aleikum brat tut coliding %s and %s \n", a->base.base.name, b->base.base.name);
    vec3 aMin, aMax, bMin, bMax;
    ColliderNodeGetWorldAABB(a, aMin, aMax);
    ColliderNodeGetWorldAABB(b, bMin, bMax);

    vec3 overlap = {
        fminf(aMax[0], bMax[0]) - fmaxf(aMin[0], bMin[0]),
        fminf(aMax[1], bMax[1]) - fmaxf(aMin[1], bMin[1]),
        fminf(aMax[2], bMax[2]) - fmaxf(aMin[2], bMin[2])
    };

    int axis = 0;
    if(overlap[1] < overlap[axis]) axis = 1;
    if(overlap[2] < overlap[axis]) axis = 2;

    float dir = (a->base.transform.position[axis] < b->base.transform.position[axis]) ? -1.0f : 1.0f;

    if(!a->isStatic && !b->isStatic) 
    {
        float resolution = overlap[axis] * 0.5f * dir;
        a->base.transform.position[axis] += resolution;
        b->base.transform.position[axis] -= resolution;
    } 
    else if(!a->isStatic) 
    {
        a->base.transform.position[axis] += overlap[axis] * dir;
    }
    else if(!b->isStatic)
    {
        b->base.transform.position[axis] -= overlap[axis] * dir;
    }

  SpatialNodeUpdateGlobalTransform((SpatialNode*) a);
  SpatialNodeUpdateGlobalTransform((SpatialNode*) b);
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
  cJSON_AddBoolToObject(root, "isStatic", node->isStatic);
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
  bool isStatic = cJSON_IsTrue(cJSON_GetObjectItem(json, "isStatic"));
  ColliderNode* node = ColliderNodeCreate(name, min, max, isTrigger, isStatic);
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
bool CheckSphereCollision(vec3 sphereCenter, float sphereRadius, vec3 boxMin, vec3 boxMax) 
{
    vec3 closestPoint;
    closestPoint[0] = fmaxf(boxMin[0], fminf(sphereCenter[0], boxMax[0]));
    closestPoint[1] = fmaxf(boxMin[1], fminf(sphereCenter[1], boxMax[1]));
    closestPoint[2] = fmaxf(boxMin[2], fminf(sphereCenter[2], boxMax[2]));
    
    float distance = glm_vec3_distance(sphereCenter, closestPoint);
    
    return distance < sphereRadius;
}
