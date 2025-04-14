#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include "spatial_node.h"
#include "model.h"

typedef enum
{
  MODEL_CUBE,
  MODEL_CYLINDER,
  MODEL_CAPSULE,
  MODEL_CUSTOM
} ShapeType;

typedef struct 
{
  SpatialNode base;
  ShapeType shapeType;
  Model model;
  char modelPath[256];
} ModelNode;

ModelNode*   ModelNodeCreate(ShapeType type, const char* name, const char* modelPath);
void  ModelNodeFree(ModelNode* node);

void ModelNodeUpdate(ModelNode* node, float delta);


#endif // !MODEL_NODE_H

