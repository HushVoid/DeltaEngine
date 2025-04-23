#ifndef COLLISION_NODE_H
#define COLLISION_NODE_H



#include "spatial_node.h"
#include <float.h>

typedef struct  
{
  SpatialNode base;
  vec3 min;
  vec3 max;
  bool isTrigger;
  bool isColliding;
  bool isStatic;
}ColliderNode;


ColliderNode* ColliderNodeCreate(const char* name, vec3 min, vec3 max, bool isTrigger, bool isStatic);
ColliderNode* ColliderNodeCreateDefault(const char* name);

void ColliderNodeGetWorldAABB(const ColliderNode* collider, vec3 outMin, vec3 outMax);

bool CheckSphereCollision(vec3 sphereCenter, float sphereRadius, vec3 boxMin, vec3 boxMax);
bool ColliderNodeCheckCollision(const ColliderNode* a, const ColliderNode* b);
void ColliderNodeHandleStateChange(ColliderNode* a, ColliderNode* b, bool state);
void ColliderNodeResolveCollision(ColliderNode* a, ColliderNode*b);

void ColliderNodeFree(ColliderNode* node);

void ColliderNodeToJSON(const ColliderNode* node, cJSON* root);
ColliderNode* ColliderNodeFromJSON(const cJSON* json);


#endif // !COLLISION_NODE_H
