#include "node.h"
#include "../include/cJSON/cJSON.h"
#include "spatial_node.h"
#include "model_node.h"
#include "camera_node.h"
#include "light_node.h"
#include "player_node.h"
#include "collision_node.h"
#include <msxml.h>
#include <string.h>


Node* NodeCreate(const char* name)
{
  Node* node = calloc(1, sizeof(Node));
  strcpy_s(node->name, sizeof(node->name), name);
  node->children = dynlistInit(sizeof(Node*), 4); 
  node->enabled = true;
  node->type = NODE_BASE;
  return node;

}

void NodeDestroy(Node* node)
{
  if(!node)
  {
    printf("NodeDestroy: Invalid node pass\n");
    return;
  }
  if(node->children)
  {
    //Recursivley delete all children
    for(int i = 0; i < node->children->size; i++)
    {
      Node* child = *(Node**)dynlistAt(node->children, i);
      NodeDestroy(child);
    }
    dynlistFree(node->children);
  }
  switch (node->type) 
  {
    case NODE_BASE:
      free(node);
      return;
    case NODE_SPATIAL:
      SpatialNodeFree((SpatialNode*)node);
      return;
    case NODE_CAMERA:
      CameraNodeFree((CameraNode*)node);
      return;
    case NODE_LIGHTD:
      DLightFree((DirectionalLightNode*)node); 
      return;
    case NODE_LIGHTP:
      PLightFree((PointLightNode*)node); 
      return;
    case NODE_LIGHTS:
      SLightFree((SpotLightNode*)node); 
      return;
    case NODE_MODEL:
      ModelNodeFree((ModelNode*)node);
      return;
    //ADD PLAYER NODE DELITION HANDLING
    case NODE_PLAYER:
      PlayerNodeFree((PlayerNode*) node);
      return;
    case NODE_COLLISION:
      ColliderNodeFree((ColliderNode*) node);
      return;
  }
}

bool NodeHasTransform(Node* node)
{
  return 
  node->type == NODE_SPATIAL ||
  node->type == NODE_MODEL ||
  node->type == NODE_CAMERA ||
  node->type == NODE_LIGHTD ||
  node->type == NODE_LIGHTP ||
  node->type == NODE_LIGHTS ||
  node->type == NODE_PLAYER ||
  node->type == NODE_COLLISION;
  
}
void NodeReparent(Node* node, Node* newParent)
{
  if(node->parent)
  {
    unsigned int index = NodeFindChildIndex(node->parent, node->name, false);
    dynlistDeleteAt(node->parent->children, index);
  }
  node->parent = newParent;
  dynlistPush(newParent->children, &node);
  if(NodeHasTransform(node))
  {
    SpatialNode* spatial = (SpatialNode*)node;
    SpatialNodeUpdateGlobalTransform(spatial);
  }
}
Node* NodeFindChild(Node* node, const char* name, bool recursive)
{
  if(!node)
  {
    printf("NodeFindChild: node passed to func is invalid\n");
    return NULL;
  }
  if(node->children->size == 0)
  {
    printf("NodeFindChild: node has not any children\n");
    return NULL;
  }
  for(int i = 0; i < node->children->size; i++)
  {
    Node* child = *(Node**)dynlistAt(node->children, i);
    if(strcmp(name, child->name) == 0)
      return child;
  }
  if(recursive)
  {
    for(int i = 0; i < node->children->size; i++)
    {
      Node* child = *(Node**)dynlistAt(node->children, i);
      Node* childFound = NodeFindChild(child, name, true);
      if(childFound)
       return childFound;
    }
  }
  printf("NodeFindChild: Child with name %s is not found\n", name);
  return NULL;
}

unsigned int NodeFindChildIndex(Node* node, const char* name, bool recursive)
{
  if(!node)
  {
    printf("NodeFindChildIndex: node passed to func is invalid\n");
    return -1;
  }
  if(node->children->size == 0)
  {
    printf("NodeFindChildIndex: node has not any children\n");
    return -1;
  }
  for(int i = 0; i < node->children->size; i++)
  {
    Node* child = *(Node**)dynlistAt(node->children, i);
    if(strcmp(name, child->name) == 0)
      return i;
  }
  if(recursive)
  {
    for(int i = 0; i < node->children->size; i++)
    {
      Node* child = *(Node**)dynlistAt(node->children, i);
      Node* childFound = NodeFindChild(child, child->name, true);
      if(childFound)
       return i;
    }
  }
  printf("NodeFindChildIndex: Child with name %s is not found\n", name);
  return -1;
}

bool NodeCanHaveChilder(Node* node)
{
  return (node->type == NODE_MODEL ||
          node->type == NODE_SPATIAL ||
          node->type == NODE_PLAYER ||
          node->type == NODE_BASE
);
}

void NodeDeleteChild(Node* parent, Node* child)
{
  if(!parent || !child)
  {
    printf("NodeDeleteChild: invalid arguments passed\n");
    return;
  }
  int childIndex = -1;
  //Finding child here by pointer because we need to compare pointers or smth i dunno 
  for(int i = 0; i < parent->children->size; i++)
  {
    Node* current = *(Node**)dynlistAt(parent->children, i);
    if(current == child)
    {
      childIndex = i;
      break;
    }
  }
  if(childIndex == -1)
  {
    printf("NodeDeleteChild: node %s is not a child of %s\n", child->name, parent->name);
    return;
  }
  dynlistDeleteAt(parent->children, childIndex);
  NodeDestroy(child);
}//By pointer

void NodeDeleteChild_Index(Node* parent, unsigned int index)
{
  if(!parent)
  {
    printf("NodeDeleteChild_Index: invalid arguments passed\n");
    return;
  }
  Node* child = *(Node**)dynlistAt(parent->children, index);
  if(!child)
  {
    printf("NodeDeleteChild_Index: cannot find child at index %d in node %s\n", index, parent->name);
    return;
  }
  dynlistDeleteAt(parent->children, index);
  NodeDestroy(child);
}//By index
void NodeAddChild(Node* parent, Node* child)
{
  if(!parent || !child)
  {
    //you actually reading all this code???
    printf("nah twin she got you blushin twin aw hell naw that's not even u twin you gotta lock up twin foenem grave bruh aeee🥀");
    return;
  }
  if(!NodeCanHaveChilder(parent))
  {
    printf("NodeAddChild: node of type %d cannot have children\n", parent->type);
    return;
  }
    child->parent = parent;
    dynlistPush(parent->children, &child); 
}
//Export import 

const char* NodeT2Str(NodeType type)
{
  switch (type)
  {
    case NODE_BASE:
    return "base";
    case NODE_MODEL:
    return "model";
    case NODE_CAMERA:
    return "camera";
    case NODE_SPATIAL:
    return "spatial";
    case NODE_PLAYER:
    return "player";
    case NODE_LIGHTD:
    return "lightd";
    case NODE_LIGHTP:
    return "lightp";
    case NODE_LIGHTS:
    return "lights";
    case NODE_COLLISION:
    return "collider";
  }
}
NodeType Str2NodeT(const char* type)
{
 if(strcmp(type, "base") == 0)
    return NODE_BASE;
 if(strcmp(type, "model") == 0)
    return NODE_MODEL;
 if(strcmp(type, "camera") == 0)
    return NODE_CAMERA;
 if(strcmp(type, "spatial") == 0)
    return NODE_SPATIAL;
 if(strcmp(type, "player") == 0)
    return NODE_PLAYER;
 if(strcmp(type, "lightd") == 0)
    return NODE_LIGHTD;
 if(strcmp(type, "lightp") == 0)
    return NODE_LIGHTP;
 if(strcmp(type, "lights") == 0)
    return NODE_LIGHTS;
  if(strcmp(type, "collider") == 0)
    return NODE_COLLISION;
  printf("No such type");
  return NODE_BASE;
}
//TODO Change all the stuff in switch case to each node co-resopnding function for each node
char* NodeToJSON(const Node* node)
{
  if(!node)
  {
    printf("NodeToJSON: node passed is invalid\n");
    return NULL;
  }
  cJSON* root = cJSON_CreateObject();
   
  cJSON_AddStringToObject(root, "type", NodeT2Str(node->type));
  cJSON_AddStringToObject(root, "name", node->name);
  cJSON_AddBoolToObject(root, "enabled", node->enabled);
      switch(node->type)
      {
        case NODE_BASE:
        break;
        case NODE_SPATIAL:
        const SpatialNode* spatial = (const SpatialNode*)node;
        SpatialNodeToJSON(spatial, root);
        break;
        case NODE_MODEL:
        const ModelNode* model = (const ModelNode*)node;
        ModelNodeToJSON(model, root);
        break;
        case NODE_CAMERA:
        const CameraNode* cam = (const CameraNode*)node;
        CameraNodeToJSON(cam, root);
        break;
        case NODE_LIGHTD:
        const DirectionalLightNode* dlight = (const DirectionalLightNode*)node;
        DLightToJSON(dlight, root);
        break;
        case NODE_LIGHTP:
        const PointLightNode* plight = (const PointLightNode*)node;
        PLightToJSON(plight, root);
        break;
        case NODE_LIGHTS:
        const SpotLightNode* splight = (const SpotLightNode*)node;
        SLightToJSON(splight, root); 
        break;
        case NODE_PLAYER:
        const PlayerNode* player = (const PlayerNode*)node;
        PlayerNodeToJSON(player, root);
        break;
        case NODE_COLLISION:
        const ColliderNode* collider = (const ColliderNode*)node;
        ColliderNodeToJSON(collider, root); 
        break;
      }
  if(node->children && node->children->size > 0)
  {
   cJSON* children = cJSON_CreateArray();
   for (int i = 0; i < node->children->size; i++) 
    {
      Node* child = *(Node**)dynlistAt(node->children, i);
      char* childJsonStr = NodeToJSON(child);
      cJSON* childJson = cJSON_Parse(childJsonStr);
      cJSON_AddItemToArray(children, childJson);
      free(childJsonStr);
    }
   cJSON_AddItemToObject(root, "children", children);
  }
  char* jsonStr = cJSON_Print(root);
  return jsonStr;
}


Node* NodeFromJSON(const cJSON* json)
{
  const char* typeStr = cJSON_GetStringValue(cJSON_GetObjectItem(json,"type"));
  NodeType type = Str2NodeT(typeStr);
  Node* node = NULL;
  
  switch(type)
  {
    case NODE_BASE:
    char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
    node = NodeCreate(name);
    break;
    case NODE_SPATIAL:
    node = (Node*)SpatialNodeFromJSON(json);
    break;
    case NODE_CAMERA:
    node = (Node*)CameraNodeFromJSON(json);
    break;
    case NODE_MODEL:
    node = (Node*)ModelNodeFromJSON(json);
    break;
    case NODE_LIGHTD:
    node = (Node*)DLightFromJSON(json);
    break;
    case NODE_LIGHTP:
    node = (Node*)PLightFromJSON(json);
    break;
    case NODE_LIGHTS:
    node = (Node*)SLightFromJSON(json);
    break;
    case NODE_PLAYER:
    node = (Node*)PlayerNodeFromJSON(json);
    case NODE_COLLISION:
    node = (Node*)ColliderNodeFromJSON(json);
  }
  cJSON* children = cJSON_GetObjectItem(json, "children");
  if(children)
  {
    cJSON* childJson;
    cJSON_ArrayForEach(childJson, children)
    {
      Node* child = NodeFromJSON(childJson);
      NodeAddChild(node, child);
    }
  }
  return node;
}
