#ifndef NODE_H
#define NODE_H
#define NODE_CAST(T, node_ptr) ((T*)((node_ptr)->type == NODE_##T ? node_ptr : NULL))

#include "../dynlist.h"
#include <stdbool.h>
#include "../include/cglm/cglm.h"
#include "../include/cJSON/cJSON.h"
 
typedef enum 
{
  NODE_BASE,
  NODE_SPATIAL,
  NODE_MODEL,
  NODE_LIGHTD,
  NODE_LIGHTP,
  NODE_LIGHTS,
  NODE_CAMERA,
  NODE_PLAYER
} NodeType;

typedef struct Node
{
  NodeType type; 
  char name[256];
  struct Node* parent;
  dynlist_t* children; // dynlist<Node*>;
  bool enabled;
} Node;


//core functions

Node* NodeCreate(const char* name);
void NodeDestroy(Node* node);
Node* NodeFindChild(Node* node, const char* name, bool recursive); 
unsigned int NodeFindChildIndex(Node* node, const char* name, bool recursive); 
void NodeAddChild(Node* parent, Node* child);

void NodeDeleteChild(Node* parent, Node* child); //By pointer
void NodeDeleteChild_Index(Node* parent, unsigned int index); //By index
void NodeReparent(Node* node, Node* newParent);
bool NodeCanHaveChilder(Node* node);

//(De)Serialisation
const char* NodeT2Str(NodeType type);
NodeType Str2NodeT(const char* type);
char* NodeToJSON(const Node* node);
Node* NodeFromJSON(const cJSON* json);

#endif // !NODE_H
