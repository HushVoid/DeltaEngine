#ifndef CAMERA_H
#define CAMERA_H
#define DEFAULT_CAM_YAW -90.0f
#define DEFAULT_CAM_PITCH 0.0f
#define DEFAULT_CAM_SPEED 10.0f
#define DEFAULT_CAM_FOV  60.0f
#define DEFAULT_SENS 0.1f

#include "macro.h"
#include "src/include/cglm/cglm.h"
#include "src/include/GL/glew.h"


enum Camera_Movement
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};


typedef struct{
  //Camera's coordinate system
  vec3 position;
  vec3 frontVec;
  vec3 upVec;
  vec3 rightVec;
  vec3 worldUpVec;

  //euler angles
  float pitch;
  float yaw;
  //camera's params
  float fov;
  float nearPlaneCoord;
  float farPlaneCoord;
  float speed;
  float sensitivity;
}Camera;



void UpdateCameraVectors(Camera* camera);
void InitializeCamera(Camera* camera, vec3 position, vec3 upVec, float yaw, float pitch, float nearPlane, float farPlane);
void GetViewMatrixFromCamera(Camera camera, mat4 view);
void UpdateCameraMovement(Camera* camera, enum Camera_Movement direction, float deltaTime);
void ProcessMouseMovement(Camera* camera, float dX, float dY, GLboolean constrainPitch);


#endif
