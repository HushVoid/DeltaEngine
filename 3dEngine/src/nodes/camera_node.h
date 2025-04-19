#ifndef CAMERA_NODE_H
#define CAMERA_NODE_H

#include "spatial_node.h"

typedef struct 
{
  SpatialNode base;
  mat4 projection;
  mat4 view;
  vec3 updir;
  float fov;
  float nearPlane;
  float farPlane;
  float aspect;
} CameraNode;

CameraNode* CameraNodeCreate(const char* name,float fov, vec3 worldUpVec, float nearPlane, float farPlane, float aspect);

void CalcViewMatFromCamera(CameraNode* camera);
void CalcProjectionMatFromCamera(CameraNode* camera);

void UpdateCameraNode(CameraNode* update, float delta);

void CameraNodeFree(CameraNode* node);

void CameraNodeToJSON(const CameraNode* node, cJSON* root);
CameraNode* CameraNodeFromJSON(const cJSON* json);

#endif // !CAMERA_NODE_H
