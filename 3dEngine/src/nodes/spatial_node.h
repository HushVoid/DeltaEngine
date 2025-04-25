#ifndef SPATIAL_NODE_H
#define SPATIAL_NODE_H

#include "node.h"
#include "components.h"

typedef struct
{
  Node base;
  Transform transform;
  bool visible;
  mat4 globalTransformMatrix;
} SpatialNode;

SpatialNode*   SpatialNodeCreate(const char* name);
void   SpatialNodeFree(SpatialNode* node);

// Transform API
void   SpatialNodeSetPos(SpatialNode* node, vec3 position);
void   SpatialNodeRotate(SpatialNode* node, vec3 axis, float radians);
void   SpatialNodeUpdateGlobalTransform(SpatialNode* node);
void   SpatialGetGlobalPos(SpatialNode* node, vec3 dest);

void   SpatialNodeUpdate(SpatialNode* node, float delta);

void SpatialNodeToJSON(const SpatialNode* node, cJSON* root);
SpatialNode* SpatialNodeFromJSON(const cJSON* json);

#endif // !SPATIAL_NODE_H
