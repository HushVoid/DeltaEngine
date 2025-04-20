#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include "spatial_node.h"
#include "../model.h"

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
  Model model;
  char modelPath[256];
  ShapeType shapeType;
} ModelNode;

ModelNode*   ModelNodeCreate(ShapeType type, const char* name, const char* modelPath);
void  ModelNodeFree(ModelNode* node);


const char* ShapeTypeToStr(ShapeType type);
ShapeType StrToShapeType(const char* string);
void ModelNodeUpdate(ModelNode* node, float delta);
void ModelNodeRender(ModelNode* node, shaderStruct* shader);

void ModelNodeToJSON(const ModelNode* node, cJSON* root);
ModelNode* ModelNodeFromJSON(const cJSON* json);


#endif // !MODEL_NODE_H

