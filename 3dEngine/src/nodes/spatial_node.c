#include "spatial_node.h"
#include "components.h"
#include "node.h"
#include "../macro.h"
#include <string.h>

SpatialNode*   SpatialNodeCreate(const char* name)
{
  SpatialNode* node = calloc(1, sizeof(SpatialNode));
  strcpy_s(node->base.name, sizeof(node->base.name), name);
  node->base.children = dynlistInit(sizeof(Node*), 4);
  node->base.type = NODE_SPATIAL;
  node->visible = true;
  TransformDefaultInit(&node->transform);
  glm_mat4_identity(node->globalTransformMatrix);
  return node;  
}

void SpatialNodeUpdateGlobalTransform(SpatialNode* node)
{
  glm_mat4_identity(node->globalTransformMatrix);
 
  mat4 rotMat;
  vec3 eulerangles = {
    DEG2RAD(node->transform.rotation[0]),
    DEG2RAD(node->transform.rotation[1]),
    DEG2RAD(node->transform.rotation[2])
  };
  glm_euler_yxz(eulerangles, rotMat);
  glm_translate(node->globalTransformMatrix, node->transform.position);
  glm_scale(node->globalTransformMatrix, node->transform.scale);
  glm_mat4_mul(node->globalTransformMatrix, rotMat, node->globalTransformMatrix);
  if(node->base.parent && node->base.parent->type == NODE_SPATIAL)
  {
    SpatialNode* parent = (SpatialNode*)node->base.parent;
    mat4 parentMatr;
    glm_mat4_copy(parent->globalTransformMatrix, parentMatr);
    glm_mat4_mul(parentMatr, node->globalTransformMatrix, node->globalTransformMatrix);
  }
}

void  SpatialNodeSetPos(SpatialNode* node, vec3 position)
{
  glm_vec3_copy(position, node->transform.position);
}
void SpatialNodeToJSON(const SpatialNode* node, cJSON* root)
{
  cJSON* transform = TransformToJSON(&node->transform);
  cJSON_AddItemToObject(root, "transform", transform);
  cJSON_AddBoolToObject(root, "visible", node->visible);
}

SpatialNode* SpatialNodeFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  SpatialNode* node = SpatialNodeCreate(name);
  cJSON* transform = cJSON_GetObjectItem(json, "transform");
  if(transform)
  {
    cJSON* pos = cJSON_GetObjectItem(transform, "position");
    cJSON* rot = cJSON_GetObjectItem(transform, "rotation");
    cJSON* scale = cJSON_GetObjectItem(transform, "scale");
    for(int i = 0; i < 3; i++)
    {
      node->transform.position[i] = (float)cJSON_GetArrayItem(pos, i)->valuedouble;
      node->transform.rotation[i] = (float)cJSON_GetArrayItem(rot, i)->valuedouble;
      node->transform.scale[i] = (float)cJSON_GetArrayItem(scale, i)->valuedouble;
    }
  }
  const cJSON* visible = cJSON_GetObjectItem(json, "visible");
  if (visible)
  {
    node->visible = cJSON_IsTrue(visible);
  }
  SpatialNodeUpdateGlobalTransform(node); 
  return node;
   
}
void   SpatialNodeFree(SpatialNode* node)
{
  if(!node)
  {
    printf("SpatialNodeFree: node isn't valid\n");
    return;
  }
  free(node);
}
