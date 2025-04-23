#ifndef PLAYER_NODE_H
#define PLAYER_NODE_H


#include "node.h"
#include "spatial_node.h"
#include "camera_node.h"
#include "../include/SDL2/SDL.h"
typedef struct
{
  SpatialNode base;
  float health;
  float speed;
  float jumpForce;
  vec3 velocity;
  vec3 upDir;
  bool gravityAffected;
  bool isGrounded;
} PlayerNode;

PlayerNode* PlayerNodeCreate(float jumpForce,float speed, const char* name, bool hasGravity);
PlayerNode* PlayerNodeCreateDefault(const char* name);

void PlayerNodeUpdate(PlayerNode* player, float delta, const Uint8 *keyboardState);
void PlayerNodeHandleMouse(PlayerNode* player, CameraNode* camera, float dx, float dy);

void PlayerNodeFree(PlayerNode* node);

void PlayerNodeToJSON(const PlayerNode* node, cJSON* root);
PlayerNode* PlayerNodeFromJSON(const cJSON* json);

#endif // !PLAYER_NODE_H
