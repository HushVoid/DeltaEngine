#ifndef CAMERA_NODE_H
#define CAMERA_NODE_H

#define CAM_DEFAULT_SPEED 10.0f
#define CAM_DEFAULT_SENS 0.1f

#include "spatial_node.h"

typedef enum
{
  CAMERA_EDIT_FORWARD,
  CAMERA_EDIT_BACKWARD,
  CAMERA_EDIT_LEFT,
  CAMERA_EDIT_RIGHT
} CameraMovementDir;

typedef struct 
{
  SpatialNode base;
  mat4 projection;
  mat4 view;
  vec3 updir;
  float speed;
  float sens;
  float fov;
  float nearPlane;
  float farPlane;
  float aspect;
} CameraNode;

CameraNode* CameraNodeCreate(const char* name,float fov, vec3 worldUpVec, float nearPlane, float farPlane, float aspect);
CameraNode* CameraNodeCreateDefault(const char* name);

void CalcViewMatFromCamera(CameraNode* camera);
void CalcProjectionMatFromCamera(CameraNode* camera);

void CameraNodeHandleWASD(CameraNode* camera, float delta, CameraMovementDir dir);
void CameraHandleMouse(CameraNode* camera, float dX, float dY, bool constrainPitch);

void CameraNodeUpdate(CameraNode* camera, float delta);

void CameraNodeFree(CameraNode* node);

void CameraNodeToJSON(const CameraNode* node, cJSON* root);
CameraNode* CameraNodeFromJSON(const cJSON* json);

#endif // !CAMERA_NODE_H
