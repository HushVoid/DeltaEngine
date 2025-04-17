#ifndef PLAYER_NODE_H
#define PLAYER_NODE_H

#include "node.h"
#include "spatial_node.h"

typedef struct
{
  SpatialNode base;
  float health;
  float speed;
  vec3 velocity;
  vec3 upDir;
  bool gravityAffected;
} PlayerNode;

PlayerNode* PlayerNodeCreate(float speed, const char* name, bool hasGravity);

void PlayerNodeUpdate(PlayerNode* node, float delta);

void PlayerNodeFree(PlayerNode* node);

void PlayerNodeToJSON(const PlayerNode* node, cJSON* root);
PlayerNode* PlayerNodeFromJSON(const cJSON* json);

#endif // !PLAYER_NODE_H
