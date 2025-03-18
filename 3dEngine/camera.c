#include "camera.h"
#include "macro.h"
#include "src/include/cglm/cglm.h"
#include "src/include/GL/glew.h"
#include "math.h"
void UpdateCameraVectors(Camera *camera)
{
  //calulating front camera vector
  vec3 front;
  front[0] = cos(DEG2RAD(camera->yaw)) * cos(DEG2RAD(camera->pitch));
  front[1] = sin(DEG2RAD(camera->pitch));
  front[2] = sin(DEG2RAD(camera->yaw)) * cos(DEG2RAD(camera->pitch));
  glm_vec3_normalize(front);
  glm_vec3_copy(front, camera->frontVec);
  //recalculating right and up camera's vectors
  vec3 right;
  glm_vec3_cross(camera->frontVec, camera->worldUpVec, right);
  glm_vec3_normalize(right);
  glm_vec3_copy(right, camera->rightVec);
  vec3 up;
  glm_vec3_cross(camera->rightVec, camera->frontVec, up);
  glm_vec3_normalize(up);
  glm_vec3_copy(up, camera->upVec);
}

void InitializeCamera(Camera *camera, vec3 position, vec3 upVec, float yaw, float pitch, float nearPlane, float farPlane)
{
  glm_vec3_copy((vec3){0.0f,0.0f,-1.0f}, camera->frontVec);
  glm_vec3_copy(position, camera->position);
  glm_vec3_copy(upVec, camera->worldUpVec);
  camera->nearPlaneCoord = nearPlane;
  camera->farPlaneCoord = farPlane;
  camera->fov = DEFAULT_CAM_FOV;
  camera->speed = DEFAULT_CAM_SPEED;
  camera->sensitivity = DEFAULT_SENS;
  camera->yaw = yaw;
  camera->pitch = pitch;
  UpdateCameraVectors(camera);
}

void GetViewMatrixFromCamera(Camera camera, mat4 view)
{
  vec3 target;
  glm_vec3_add(camera.position, camera.frontVec, target);
  glm_lookat(camera.position,target, camera.upVec, view);
} 
void UpdateCameraMovement(Camera* camera, enum Camera_Movement direction, float deltaTime)
{
  float velocity = camera->speed * deltaTime;
  vec3 addVecZ;
  vec3 addVecX;
  if(direction == FORWARD)
  {
    glm_vec3_scale(camera->frontVec, velocity, addVecZ);
    glm_vec3_add(camera->position, addVecZ, camera->position);
  }
  if(direction == BACKWARD)
  {
    glm_vec3_scale(camera->frontVec, velocity, addVecZ);
    glm_vec3_sub(camera->position, addVecZ, camera->position);
  }
  if(direction == LEFT)
  {
    glm_vec3_scale(camera->rightVec, velocity, addVecX);
    glm_vec3_sub(camera->position, addVecX, camera->position);
  }
  if(direction == RIGHT)
  {
    glm_vec3_scale(camera->rightVec, velocity, addVecX);
    glm_vec3_add(camera->position, addVecX, camera->position);
  }
}
void ProcessMouseMovement(Camera *camera, float dX, float dY, GLboolean constrainPitch)
{
  dX *= camera->sensitivity;
  dY *= camera->sensitivity;
  camera->yaw += dX;
  camera->pitch -= dY;
  if(constrainPitch)
  {   
    if(camera->pitch > 89.0f)
    {
      camera->pitch = 89.0f;
    }
    if(camera->pitch < -89.0)
    {
      camera->pitch = -89.0f;
    }
  }

  UpdateCameraVectors(camera);
}
