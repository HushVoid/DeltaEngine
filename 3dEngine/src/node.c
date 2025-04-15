#include "node.h"
#include "spatial_node.h"
#include "model_node.h"
#include "camera_node.h"
#include "light_node.h"
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
      return;
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
      Node* childFound = NodeFindChild(child, child->name, true);
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
    return NULL;
  }
  if(node->children->size == 0)
  {
    printf("NodeFindChildIndex: node has not any children\n");
    return NULL;
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
  return NULL;
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

