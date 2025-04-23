#include "player_node.h"
#include "spatial_node.h"


PlayerNode* PlayerNodeCreate(float jumpForce,float speed, const char* name, bool hasGravity)
{
  PlayerNode* node = calloc(1, sizeof(PlayerNode));
  strcpy_s(node->base.base.name, sizeof(node->base.base.name), name);
  node->base.base.children = dynlistInit(sizeof(Node*), 4);
  node->base.visible = true;
  node->base.base.type = NODE_MODEL;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
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
  node->base.base.type = NODE_MODEL;
  TransformDefaultInit(&node->base.transform);
  glm_mat4_identity(node->base.globalTransformMatrix);
  node->speed = 30.0f;
  node->jumpForce = 5.0f;
  node->gravityAffected = true;
  return node;

}
void PlayerNodeUpdate(PlayerNode* player, float delta, const Uint8* keyboardState)
{
   if (!player) return;
    
    Transform* t = &player->base.transform;
    
    player->velocity[0] = 0;
    player->velocity[2] = 0;
    
    vec3 moveDir = {0};
    
    if (keyboardState[SDL_SCANCODE_W])
    {
        moveDir[2] -= 1.0f; 
    }
    if (keyboardState[SDL_SCANCODE_S]) 
    {
        moveDir[2] += 1.0f;     
    }
    if (keyboardState[SDL_SCANCODE_A]) 
    {
        moveDir[0] -= 1.0f;     
    }
    if (keyboardState[SDL_SCANCODE_D]) 
    {
        moveDir[0] += 1.0f;     
    }
    
    if (glm_vec3_norm2(moveDir) > 0)
    {
        glm_vec3_normalize(moveDir);
    }
    
    vec3 forward, right;
    TransformGetForwardNoPitch(t, forward); 
    glm_vec3_cross(forward, player->upDir, right);
    glm_vec3_normalize(right);
    
    glm_vec3_scale(forward, moveDir[2] * player->speed, forward);
    glm_vec3_scale(right, moveDir[0] * player->speed, right);
    
    
    glm_vec3_add(forward, right, player->velocity);
    
    
    if (keyboardState[SDL_SCANCODE_SPACE] && player->isGrounded) 
    {
        player->velocity[1] = player->jumpForce;
        player->isGrounded = false;
    }
    
    if (player->gravityAffected && !player->isGrounded)
    {
        player->velocity[1] -= 9.8f * delta; // g = 9.8 m/s^2
    }
    
    vec3 deltaMove;
    glm_vec3_scale(player->velocity, delta, deltaMove);
    glm_vec3_add(t->position, deltaMove, t->position);
    
    SpatialNodeUpdateGlobalTransform((SpatialNode*)player);
    if(player->isGrounded)
    {
     player->velocity[1] = 0; 
    }
  
}
void PlayerNodeHandleMouse(PlayerNode* player, CameraNode* camera, float dx, float dy)
{
  float modDX = dx * camera->sens;  
  float modDY = dy * camera->sens;  
  

  player->base.transform.rotation[0] -= modDY;
  camera->base.transform.rotation[1] -= modDX;

  camera->base.transform.rotation[0] = glm_clamp(camera->base.transform.rotation[0], -89.0f, 89.0f);
}
void PlayerNodeToJSON(const PlayerNode* node, cJSON* root)
{
  SpatialNodeToJSON((const SpatialNode*)node, root);
  cJSON_AddBoolToObject(root, "gravityAffected", node->gravityAffected);
  cJSON_AddNumberToObject(root, "speed", node->speed);
  cJSON_AddNumberToObject(root, "jump", node->jumpForce);
}
PlayerNode* PlayerNodeFromJSON(const cJSON* json)
{
  char* name = cJSON_GetStringValue(cJSON_GetObjectItem(json,"name"));
  float speed = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "speed"));
  float jumpForce = (float)cJSON_GetNumberValue(cJSON_GetObjectItem(json, "jump"));
  bool isGravity = cJSON_IsTrue(cJSON_GetObjectItem(json,"object"));
  PlayerNode* node = PlayerNodeCreate(jumpForce, speed, name, isGravity);
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
