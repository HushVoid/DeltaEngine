#ifndef LIGHT_NODE_H
#define LIGHT_NODE_H

#include "spatial_node.h"
#include "light.h"

typedef struct
{
  SpatialNode base;
  DirLight light;
  float intencity;
} DirectionalLightNode;

typedef struct 
{
  SpatialNode base;
  PointLight light;
  float intencity;
} PointLightNode;

typedef struct
{
  SpatialNode base;
  SpotLight light;
  float intencity;
  float distance;
} SpotLightNode;

DirectionalLightNode* DLightCreate(const char* name, float intencity, vec3 direction);
PointLightNode* PLightCreate(const char* name, float intencity, vec3 direction);
SpotLightNode* SLightCreate(const char* name, float intencity);

#endif // !LIGHT_NODE_H
