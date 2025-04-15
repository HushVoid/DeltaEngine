#ifndef CAMERA_NODE_H
#define CAMERA_NODE_H

#include "spatial_node.h"

typedef struct 
{
  SpatialNode base;
  mat4 projection;
  mat4 view;
  float fov;
  float nearPlane;
  float farPlane;
  float aspect;
} CameraNode;

CameraNode* CreateCameraNode(const char* name, vec3 worldUpVec, float nearPlane, float farPlane);

void UpdateCameraVectors(CameraNode *camera);
void GetViewMatrixFromCamera(CameraNode camera, mat4 view);

void UpdateCameraNode(CameraNode* update, float delta);

void CameraNodeFree(CameraNode* node);

#endif // !CAMERA_NODE_H
