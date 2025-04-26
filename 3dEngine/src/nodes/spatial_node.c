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
    mat4 localTransform;
    glm_mat4_identity(localTransform);

    vec3 eulerAngles = 
    {
        DEG2RAD(node->transform.rotation[0]),
        DEG2RAD(node->transform.rotation[1]),
        DEG2RAD(node->transform.rotation[2])
    };
    
    mat4 rotMat;
    glm_euler_yxz(eulerAngles, rotMat);      
    glm_translate(localTransform, node->transform.position);
    glm_mat4_mul(localTransform, rotMat, localTransform); 
    glm_scale(localTransform, node->transform.scale);    
    if (node->base.parent && NodeHasTransform(node->base.parent))
    {
        SpatialNode* parent = (SpatialNode*)node->base.parent;
        glm_mat4_mul(parent->globalTransformMatrix, localTransform, node->globalTransformMatrix);
    } else {
        glm_mat4_copy(localTransform, node->globalTransformMatrix);
    }
    for(int i = 0; i < node->base.children->size; i++)
    {
      Node * child = *(Node**)dynlistAt(node->base.children, i);
      if(NodeHasTransform(child))
      {
        SpatialNode* spatial = (SpatialNode*) child;
        SpatialNodeUpdateGlobalTransform(spatial); 
      }
    }

}
void SpatialGetGlobalPos(SpatialNode* node, vec3 dest)
{
  vec3 globalPos;
  globalPos[0] = node->globalTransformMatrix[3][0];
  globalPos[1] = node->globalTransformMatrix[3][1];
  globalPos[2] = node->globalTransformMatrix[3][2];
  glm_vec3_copy(globalPos, dest);
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
