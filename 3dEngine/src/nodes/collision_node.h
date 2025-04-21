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
}ColliderNode;


ColliderNode* ColliderNodeCreate(const char* name, vec3 min, vec3 max, bool isTrigger);
ColliderNode* ColliderNodeCreateDefault(const char* name);

void ColliderNodeGetWorldAABB(const ColliderNode* collider, vec3 outMin, vec3 outMax);

bool ColliderNodeCheckCollision(const ColliderNode* a, const ColliderNode* b);

void ColliderNodeFree(ColliderNode* node);

void ColliderNodeToJSON(const ColliderNode* node, cJSON* root);
ColliderNode* ColliderNodeFromJSON(const cJSON* json);


#endif // !COLLISION_NODE_H
