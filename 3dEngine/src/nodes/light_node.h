#ifndef LIGHT_NODE_H
#define LIGHT_NODE_H

#include "spatial_node.h"
#include "../light.h"


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
  int id;
  float intencity;
  float radius;
} PointLightNode;

typedef struct
{
  SpatialNode base;
  SpotLight light;
  int id;
  float intencity;
} SpotLightNode;



DirectionalLightNode* DLightCreate(const char* name, float intencity, vec3 direction);
PointLightNode* PLightCreate(const char* name, float intencity, float radius, int id);
SpotLightNode* SLightCreate(const char* name, float intencity, vec3 direction, int id);

DirectionalLightNode* DLightCreateDefault(const char* name);
PointLightNode* PLightCreateDefault(const char* name);
SpotLightNode* SLightCreateDefault(const char* name);

void DLightFree(DirectionalLightNode* light);
void PLightFree(PointLightNode* light);
void SLightFree(SpotLightNode* light);

void DLightToJSON(const DirectionalLightNode* light, cJSON* root);
void PLightToJSON(const PointLightNode* light, cJSON* root);
void SLightToJSON(const SpotLightNode* light, cJSON* root);

DirectionalLightNode* DLightFromJSON(const cJSON* json);
PointLightNode* PLightFromJSON(const cJSON* json);
SpotLightNode* SLightFromJSON(const cJSON* json);

//INTERNAL USE ONLY
void PointLightCalc(PointLightNode* light);
#endif // !LIGHT_NODE_H
