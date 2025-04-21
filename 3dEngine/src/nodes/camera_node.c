#include "camera_node.h"
#include "components.h"
#include "node.h"
#include "spatial_node.h"

CameraNode* CameraNodeCreate(const char* name, float fov,  vec3 worldUpVec, float nearPlane, float farPlane, float aspect)
{
  CameraNode* node = calloc(1, sizeof(CameraNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.visible = true;
  node->base.base.type = NODE_CAMERA;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  glm_vec3_copy(worldUpVec, node->updir);
  node->speed = CAM_DEFAULT_SPEED;
  node->sens = CAM_DEFAULT_SENS;
  node->fov = fov;
  node->nearPlane = nearPlane;
  node->farPlane = farPlane;
  node->aspect = aspect;
  CalcViewMatFromCamera(node);
  CalcProjectionMatFromCamera(node);
  return node;
}

CameraNode* CameraNodeCreateDefault(const char* name)
{
  CameraNode* node = calloc(1, sizeof(CameraNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.visible = true;
  node->base.base.type = NODE_CAMERA;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  glm_vec3_copy((vec3){0, 1, 0}, node->updir);
  node->speed = CAM_DEFAULT_SPEED;
  node->sens = CAM_DEFAULT_SENS;
  node->fov = 60;
  node->nearPlane = 0.125f;
  node->farPlane = 100.0f;
  node->aspect = 16.0/9.0;
  CalcViewMatFromCamera(node);
  CalcProjectionMatFromCamera(node);
  return node;
}

void CalcViewMatFromCamera(CameraNode* camera)
{
  vec3 forward;
  TransformGetForward(&camera->base.transform, forward);
  vec3 target;
  glm_vec3_add(camera->base.transform.position, forward, target);
  glm_lookat(camera->base.transform.position, target, camera->updir, camera->view);
}
void CalcProjectionMatFromCamera(CameraNode* camera)
{
 glm_perspective(DEG2RAD(camera->fov), camera->aspect, camera->nearPlane, camera->farPlane, camera->projection); 
}
void CameraNodeToJSON(const CameraNode* node, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)node, root);
  cJSON_AddNumberToObject(root, "fov", node->fov); 
  cJSON_AddNumberToObject(root, "nearplane", node->nearPlane); 
  cJSON_AddNumberToObject(root, "farplane", node->farPlane); 
  cJSON_AddNumberToObject(root, "aspect", node->aspect); 
}

//Hande EDIT mode Camera movement;
void CameraNodeHandleWASD(CameraNode* camera, float delta, CameraMovementDir dir)
{
  float dSpeed = camera->speed * delta;
  vec3 fwDir;
  TransformGetForward(&camera->base.transform, fwDir);
  glm_vec3_scale(fwDir, dSpeed, fwDir);
  vec3 rightDir;
  TransformGetRightVec(&camera->base.transform, camera->updir, rightDir);
  glm_vec3_scale(rightDir, dSpeed, rightDir);
  if(dir == CAMERA_EDIT_FORWARD)
    glm_vec3_add(camera->base.transform.position, fwDir, camera->base.transform.position);
  if(dir == CAMERA_EDIT_BACKWARD)
    glm_vec3_sub(camera->base.transform.position, fwDir, camera->base.transform.position);
  if(dir == CAMERA_EDIT_RIGHT)
    glm_vec3_add(camera->base.transform.position, rightDir, camera->base.transform.position);
  if(dir == CAMERA_EDIT_LEFT)
    glm_vec3_sub(camera->base.transform.position, rightDir, camera->base.transform.position);
}
//Handle EDIT mode Camera movement; 
void CameraHandleMouse(CameraNode* camera, float dX, float dY, bool constrainPitch)
{
  float modDX = dX * camera->sens;  
  float modDY = dY * camera->sens;  
  

  camera->base.transform.rotation[0] -= modDY;
  camera->base.transform.rotation[1] -= modDX;

  if(constrainPitch)
  {
    camera->base.transform.rotation[0] = glm_clamp(camera->base.transform.rotation[0], -89.0f, 89.0f);
  }
}
CameraNode* CameraNodeFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  float near = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "nearplane"));  
  float fov = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "fov"));  
  float far = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "farplane"));  
  float aspect = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "aspect"));
  CameraNode* node = CameraNodeCreate(name, fov, (vec3){0, 1, 0}, near, far, aspect);
  cJSON* transform = cJSON_GetObjectItem(json, "transform");
  if(transform)
  {
    cJSON* pos = cJSON_GetObjectItem(transform, "position");
    cJSON* rot = cJSON_GetObjectItem(transform, "rotation");
    cJSON* scale = cJSON_GetObjectItem(transform, "scale");
    for(int i = 0; i < 3; i++)
    {
      node->base.transform.position[i] = (float)cJSON_GetArrayItem(pos, i)->valuedouble;
      node->base.transform.rotation[i] = (float)cJSON_GetArrayItem(rot, i)->valuedouble;
      node->base.transform.scale[i] = (float)cJSON_GetArrayItem(scale, i)->valuedouble;
    }
  }
  SpatialNodeUpdateGlobalTransform((SpatialNode*)node);
  return node;
}


void CameraNodeFree(CameraNode* node)
{
  if(!node)
  {
    printf("CameraNodeFree: node isn't valid \n");
    return;
  }
  free(node);
}
