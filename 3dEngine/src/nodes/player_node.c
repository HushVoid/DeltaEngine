#include "player_node.h"
#include "node.h"
#include "spatial_node.h"


PlayerNode* PlayerNodeCreate(float jumpForce,float speed, const char* name, bool hasGravity)
{
  PlayerNode* node = calloc(1, sizeof(PlayerNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.visible = true;
  node->base.base.type = NODE_PLAYER;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  glm_vec3_copy((vec3){0,1,0}, node->upDir);
  node->speed = speed;
  node->jumpForce = jumpForce;
  node->gravityAffected = hasGravity;
  return node;
}
PlayerNode* PlayerNodeCreateDefault(const char* name)
{
  PlayerNode* node = calloc(1, sizeof(PlayerNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.visible = true;
  node->base.base.type = NODE_PLAYER;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  glm_vec3_copy((vec3){0,1,0}, node->upDir);
  node->speed = 10.0f;
  node->jumpForce = 5.0f;
  node->gravityAffected = true;
  return node;

}
void PlayerNodeUpdate(PlayerNode* player, float delta, PlayerMoveDir dir)
{
   if (!player) return;
    
    player->velocity[0] = 0; 
    player->velocity[2] = 0; 
    vec3 moveDir = {0};
    
    if (dir == PLAYER_FORWARD)
    {
        moveDir[2] = 1.0f; 
    }
    if (dir == PLAYER_BACKWARD) 
    {
        moveDir[2] = -1.0f;     
    }
    if (dir == PLAYER_LEFT) 
    {
        moveDir[0] = -1.0f;     
    }
    if (dir == PLAYER_RIGHT) 
    {
        moveDir[0] = 1.0f;     
    }
    
    if (glm_vec3_norm2(moveDir) > 0)
    {
        glm_vec3_normalize(moveDir);
    }
    
    vec3 forward, right;
    TransformGetForwardNoPitch(&player->base.transform, forward); 
    glm_vec3_cross(forward, player->upDir, right);
    glm_vec3_normalize(right);
    
    glm_vec3_scale(forward, moveDir[2] * player->speed, forward);
    glm_vec3_scale(right, moveDir[0] * player->speed, right);
    
    vec3 force;
    glm_vec3_add(forward, right, force);
    glm_vec3_add(force, player->velocity, player->velocity);
    
    
    if (dir == PLAYER_JUMP && player->isGrounded) 
    {
        player->velocity[1] = player->jumpForce;
        player->isGrounded = false;
    }


    vec3 deltaMove;
    glm_vec3_scale(player->velocity, delta, deltaMove);
    glm_vec3_add(player->base.transform.position, deltaMove, player->base.transform.position);
    SpatialNodeUpdateGlobalTransform((SpatialNode*)player);
    player->velocity[0] = 0; 
    player->velocity[2] = 0; 
}
void PlayerNodePhysicsStep(PlayerNode* player, float delta)
{
    if (player->gravityAffected && !player->isGrounded) {
        player->velocity[1] -= 9.8f * delta; // g = 9.8 m/s²
    }
    const float maxFallSpeed = -50.0f;
    const float maxRiseSpeed = 100.0f;
    player->velocity[1] = glm_clamp(player->velocity[1], maxFallSpeed, maxRiseSpeed);

    if (!player->isGrounded) {
        const float airResistance = 0.02f;
        player->velocity[0] *= (1.0f - airResistance);
        player->velocity[2] *= (1.0f - airResistance);
    }

    vec3 deltaMove;
    glm_vec3_scale(player->velocity, delta, deltaMove);
    glm_vec3_add(player->base.transform.position, deltaMove, player->base.transform.position);


    if (player->isGrounded) {
        player->velocity[1] = 0.0f;
    }

    SpatialNodeUpdateGlobalTransform((SpatialNode*)player);
}
void PlayerNodeHandleMouse(PlayerNode* player, CameraNode* camera, float dx, float dy)
{
  float modDX = dx * camera->sens;  
  float modDY = dy * camera->sens;  
  

  player->base.transform.rotation[1] -= modDX;
  camera->base.transform.rotation[1] -= modDX;
  camera->base.transform.rotation[0] -= modDY;

  camera->base.transform.rotation[0] = glm_clamp(camera->base.transform.rotation[0], -89.0f, 89.0f);
  SpatialNodeUpdateGlobalTransform((SpatialNode*)player);
}
void PlayerNodeToJSON(const PlayerNode* node, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)node, root);
  cJSON_AddBoolToObject(root, "gravityAffected", node->gravityAffected);
  cJSON_AddNumberToObject(root, "speed", node->speed);
  cJSON_AddNumberToObject(root, "jump", node->jumpForce);
  cJSON_AddBoolToObject(root, "isActive", node->isActive);
}
PlayerNode* PlayerNodeFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  float speed = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "speed"));
  float jumpForce = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "jump"));
  bool isGravity = cJSON_IsTrue(cJSON_GetObjectItem(json,"gravityAffected"));
  bool isActive = cJSON_IsTrue(cJSON_GetObjectItem(json,"isActive"));
  PlayerNode* node = PlayerNodeCreate(jumpForce, speed, name, isGravity);
  node->isActive = isActive;
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

void PlayerNodeFree(PlayerNode* node)
{
  if(!node)
  {
    printf("PlayerNodeFree: node isn't valid\n");
    return;
  }
  free(node);  
}

PlayerNode* PlayerNodeClone(const PlayerNode* src)
{
  PlayerNode* dest = PlayerNodeCreateDefault(src->base.base.name);
  memcpy(&dest->base, &src->base, sizeof(SpatialNode));
  dest->speed = src->speed;
  dest->isGrounded = src->isGrounded;
  dest->health = src->health;
  dest->gravityAffected = src->gravityAffected;
  glm_vec3_copy(src->upDir, dest->upDir);
  dest->jumpForce = src->jumpForce;
  return dest;
}
