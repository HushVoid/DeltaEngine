#ifndef PLAYER_NODE_H
#define PLAYER_NODE_H


#include "node.h"
#include "spatial_node.h"
#include "camera_node.h"
#include "../include/SDL2/SDL.h"

typedef enum
{
  PLAYER_STILL,
  PLAYER_FORWARD,
  PLAYER_BACKWARD,
  PLAYER_LEFT,
  PLAYER_RIGHT,
  PLAYER_JUMP
} PlayerMoveDir;

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
  bool isActive;
} PlayerNode;

PlayerNode* PlayerNodeCreate(float jumpForce,float speed, const char* name, bool hasGravity);
PlayerNode* PlayerNodeCreateDefault(const char* name);

void PlayerNodeHandleMouse(PlayerNode* player, CameraNode* camera, float dx, float dy);
void PlayerNodeUpdate(PlayerNode* player, float delta, PlayerMoveDir dir);
void PlayerNodePhysicsStep(PlayerNode* player, float delta);

PlayerNode* PlayerNodeClone(const PlayerNode* src);

void PlayerNodeFree(PlayerNode* node);

void PlayerNodeToJSON(const PlayerNode* node, cJSON* root);
PlayerNode* PlayerNodeFromJSON(const cJSON* json);

#endif // !PLAYER_NODE_H
