#include "ui.h"
#include "gpu_light.h"
#include "include/generated/dcimgui.h"
#include "model.h"
#include "nodes/camera_node.h"
#include "nodes/light_node.h"
#include "nodes/model_node.h"
#include "nodes/node.h"
#include "nodes/player_node.h"
#include "nodes/spatial_node.h"
#include "scene.h"
#include <string.h>


void DrawSceneInspector(Scene** scene)
{
  if(!scene || !*scene)
    return;
  ImGui_Begin("Scene inspector", NULL, ImGuiWindowFlags_None);
  static char path_buf[256];
  ImGui_InputText("Save/Load path", path_buf, sizeof(path_buf), ImGuiInputTextFlags_None);
  ImGui_Text("Spot lights count: %d", (*scene)->lights->spotLightsCount);
  ImGui_Text("Point lights count: %d", (*scene)->lights->pointLightsCount);
  ImGui_Checkbox("Scene enable light", &(*scene)->enableLights);

  static int screenSize[2];
  CameraNode* camera = (*scene)->activeCamera;
  ImGui_DragFloat("Fov", &camera->fov);
  ImGui_DragFloat("Near frustum plane", &camera->nearPlane);
  ImGui_DragFloat("Far frusutum plane", &camera->farPlane);
  if(ImGui_InputInt2("Screen width height", screenSize, ImGuiInputTextFlags_None))
  {
    if(screenSize[0] < 0 || screenSize[1] < 0)
    {
     screenSize[0] = 0; 
     screenSize[1] = 0; 
    }
  }
  if(screenSize[1] > 0)
    camera->aspect = (float)screenSize[0] / (float)screenSize[1];
  CalcProjectionMatFromCamera(camera);
  if(ImGui_Button("Save scene"))
  {
    SceneSave(*scene, path_buf);
  }
  if(ImGui_Button("Load scene"))
  {
   Scene* newScene = SceneLoad(path_buf);
   Scene* oldScene = *scene;
    if(newScene)
    {
      if(newScene != *scene)
      {
        *scene = newScene;
        SceneDestroy(oldScene);
      }
      ImGui_Text("Scene loaded successfully at path %s", path_buf);
    }
  }      
  ImGui_End();
}
void DrawSceneHierarchy(Scene* scene, Node** forSelection, GLuint ubo)
{
  ImGui_Begin("Scene hierarchy", NULL, ImGuiWindowFlags_None);
  DrawNodeTree(scene->root, forSelection);
  bool openPopUp = false;

  if(ImGui_BeginPopupContextWindow())
  {
    if(ImGui_MenuItem("Create base"))
    {
      Node* newNode = NodeCreate("New node");
      NodeAddChild(scene->root, newNode);
    }
    if(ImGui_MenuItem("Create Spatial"))
    {
      SpatialNode* newNode = SpatialNodeCreate("New spatial");
      NodeAddChild(scene->root, (Node*)newNode);
    }
    if(ImGui_MenuItem("Create Camera"))
    {
      CameraNode* newNode = CameraNodeCreateDefault("Camera");
      NodeAddChild(scene->root, (Node*)newNode);
    }
    if(ImGui_MenuItem("Create Directional light"))
    {
      DirectionalLightNode* newNode = DLightCreateDefault("Dir light");
      scene->lights->dirLight = GPUDirLightCreate(newNode);
      NodeAddChild(scene->root, (Node*)newNode);
    }
    if(ImGui_MenuItem("Create Point light"))
    {
      if(scene->lights->pointLightsCount < MAX_POINT_LIGHTS)
      {
        PointLightNode* newNode = PLightCreateDefault("Point light");
        newNode->id = scene->lights->nextLightid++;
        scene->lights->pointLightsCount++;
        scene->lights->lightsDirty = true;
        GPUPointLight* light = GPUPointLightCreate(newNode);
        dynlistPush(scene->lights->pointLights, &light);
        NodeAddChild(scene->root, (Node*)newNode);
      }
    }
    if(ImGui_MenuItem("Create Spot light"))
    {
      if(scene->lights->spotLightsCount < MAX_SPOT_LIGHTS)
      {
        SpotLightNode* newNode = SLightCreateDefault("Spot light");
        newNode->id = scene->lights->nextLightid++;
        scene->lights->spotLightsCount++;
        scene->lights->lightsDirty = true;
        GPUSpotLight* light = GPUSpotLightCreate(newNode);
        dynlistPush(scene->lights->spotLights, &light);
        NodeAddChild(scene->root, (Node*)newNode);
      }
    }
    if(ImGui_MenuItem("Create Collider"))
    {
      ColliderNode* newNode = ColliderNodeCreateDefault("Collider");
      NodeAddChild(scene->root, (Node*)newNode);
    }
    if(ImGui_MenuItem("Create Cube"))
    {
      ModelNode* newNode = ModelNodeCreate(MODEL_CUBE, "Cube", NULL);
      Node* newNodeN = (Node*) newNode;
      NodeAddChild(scene->root, newNodeN);
      dynlistPush(scene->renderQueue, &newNodeN);
    }
    if(ImGui_MenuItem("Create Cylinder"))
    {
      ModelNode* newNode = ModelNodeCreate(MODEL_CYLINDER, "Cylinder", NULL);
      Node* newNodeN = (Node*) newNode;
      NodeAddChild(scene->root, newNodeN);
      dynlistPush(scene->renderQueue, &newNodeN);
    }
    if(ImGui_MenuItem("Create Capsule"))
    {
      ModelNode* newNode = ModelNodeCreate(MODEL_CAPSULE, "Capsule", NULL);
      Node* newNodeN = (Node*) newNode;
      NodeAddChild(scene->root, newNodeN);
      dynlistPush(scene->renderQueue, &newNodeN);
    }
    if(ImGui_MenuItem("Create Custom model..."))
    {
      openPopUp = true;
    }
    if(ImGui_MenuItem("Create Player node"))
    {
      PlayerNode* newNode = PlayerNodeCreateDefault("Player");
      NodeAddChild(scene->root, (Node*)newNode);
    }
    if(ImGui_MenuItem("DeleteNode"))
    {
      if(*forSelection && *forSelection != scene->root)
      {
        Node* nodeDelete = *forSelection;
        Node* parent = nodeDelete->parent;
        if(parent)
        {
          NodeDeleteChild(parent, nodeDelete);
          NodeDestroyWithLightCleanup(nodeDelete, scene, ubo);
        }else 
        {
          NodeDestroyWithLightCleanup(nodeDelete, scene, ubo);
        }
        *forSelection = NULL;
      }
    }
    ImGui_EndPopup();
  }


  if(openPopUp)
    ImGui_OpenPopup("Custom Model Path", ImGuiPopupFlags_None);
  
  if(ImGui_BeginPopup("Custom Model Path", ImGuiWindowFlags_None))
  {
    static char pathBuffer[256] = "";
    ImGui_InputText("Model Path", pathBuffer, sizeof(pathBuffer), ImGuiInputTextFlags_None);
    if(ImGui_Button("Create"))
    {
      if(strcmp(pathBuffer, "") > 0)
      {
        ModelNode* newNode = ModelNodeCreate(MODEL_CUSTOM, "Custom model", pathBuffer);    
        Node* newNodeN = (Node*) newNode;
        NodeAddChild(scene->root, newNodeN);
        dynlistPush(scene->renderQueue, &newNodeN);
        pathBuffer[0] = '\0';
        ImGui_CloseCurrentPopup();
      }
    }
    if(ImGui_Button("Cancel"))
    {
      pathBuffer[0] = '\0';
      ImGui_CloseCurrentPopup();
    }
    ImGui_EndPopup();
  }
  ImGui_End();
}
void DrawNodeTree(Node* node, Node** nodeSelected)
{
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
  if(node == *nodeSelected) flags |= ImGuiTreeNodeFlags_Selected;
  if(node->children == NULL || node->children->size == 0) flags |= ImGuiTreeNodeFlags_Leaf;

  bool isOpen = ImGui_TreeNodeExPtr(node, flags, "%s (%s)", node->name, NodeT2Str(node->type));
  if(ImGui_IsItemClicked())
  {
    if(*nodeSelected)
    {
      (*nodeSelected)->isSelected = false;
    }
    *nodeSelected = node;
    (*nodeSelected)->isSelected = true;
  } 

  if(ImGui_BeginDragDropSource(ImGuiDragDropFlags_None))
  {
    ImGui_SetDragDropPayload("NODE_HIERARCHY", &node, sizeof(Node*), ImGuiCond_None);
    ImGui_Text("Moving %s", node->name);
    ImGui_EndDragDropSource();
  }
  if(ImGui_BeginDragDropTarget())
  {
    const ImGuiPayload* payload;
    if((payload = ImGui_AcceptDragDropPayload("NODE_HIERARCHY", ImGuiDragDropFlags_None)))
    {
      Node* draggedNode = *(Node**)payload->Data;
      if(draggedNode != node && NodeCanHaveChildren(node))
      {
        NodeReparent(draggedNode, node);
      }
    }
    ImGui_EndDragDropTarget();
  }
  if(isOpen)
  {
    if(node->children)
    {
      for(int i = 0; i < node->children->size; i++)
      {
        Node* child = *(Node**)dynlistAt(node->children, i);
        if(strcmp(child->name,"MAINCAM") == 0)
          continue;
        DrawNodeTree(child, nodeSelected);
      }
    }
    ImGui_TreePop();
  }
}
void DrawNodeInspector(Node* node, Scene* scene)
{
  if(!node || !scene)
  {
    return;
  }
  ImGui_Begin("Inspector", NULL, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui_Text("Editing: %s", node->name);

  static char name_buf[256];
  strcpy_s(name_buf, sizeof(name_buf), node->name);
  if(ImGui_InputText("Name", name_buf, sizeof(name_buf), ImGuiInputTextFlags_None))
  {
   node->name[0] = '\0';
   strcpy_s(node->name, sizeof(node->name), name_buf);
  }
  if(NodeHasTransform(node))
  {
    SpatialNode* spatial = (SpatialNode*)node;
    ImGui_DragFloat3("Position", spatial->transform.position);
    ImGui_DragFloat3("Rotation", spatial->transform.rotation);
    ImGui_DragFloat3("Scale", spatial->transform.scale);
    if(node->type == NODE_CAMERA)
    {
      static int screenSize[2];
      CameraNode* camera = (CameraNode*)node;
      ImGui_DragFloat("Fov", &camera->fov);
      ImGui_DragFloat("Near frustum plane", &camera->nearPlane);
      ImGui_DragFloat("Far frusutum plane", &camera->farPlane);
      if(ImGui_InputInt2("Screen width height", screenSize, ImGuiInputTextFlags_None))
      {
        if(screenSize[0] < 0 || screenSize[1] < 0)
        {
         screenSize[0] = 0; 
         screenSize[1] = 0; 
        }
      }
      if(screenSize[1] > 0)
        camera->aspect = (float)screenSize[0] / (float)screenSize[1];
      ImGui_Checkbox("Is active", &camera->isActive); 
      CalcProjectionMatFromCamera(camera);
    }
    if(node->type == NODE_MODEL)
    {
      ModelNode* model = (ModelNode*)node;
      if(model->shapeType == MODEL_CUSTOM)
      {      
        static char path_buf[256];
        strcpy_s(path_buf, sizeof(path_buf), model->modelPath);
        ImGui_InputText("Model path", path_buf, sizeof(path_buf), ImGuiInputTextFlags_None);
        if(ImGui_Button("reload"))
        {
          model->modelPath[0] = '\0';
          strcpy_s(model->modelPath, sizeof(model->modelPath), path_buf);
          DeleteModel(&model->model);
          ModelInit(&model->model, path_buf);
        }
        
      }
    }
    if(node->type == NODE_LIGHTD)
    {
      DirectionalLightNode* light = (DirectionalLightNode*) node;
      ImGui_DragFloat("Intencity", &light->intencity);  
      ImGui_ColorEdit3("Color", light->light.color, ImGuiColorEditFlags_None);  
      ImGui_DragFloat3("Direction", light->light.direction);
      LMUpdateGPUDLight(scene->lights, light);
    }
    if(node->type == NODE_LIGHTP)
    {
      PointLightNode* light = (PointLightNode*) node;
      ImGui_Text("Lights id: %d", light->id);
      ImGui_DragFloat("Intencity", &light->intencity);  
      ImGui_ColorEdit3("Color", light->light.color, ImGuiColorEditFlags_None);  
      ImGui_DragFloat("Radius", &light->radius);
      PointLightCalc(light);
    }
    if(node->type == NODE_LIGHTS)
    {
      SpotLightNode* light = (SpotLightNode*) node;
      ImGui_Text("Lights id: %d", light->id);
      ImGui_DragFloat("Intencity", &light->intencity);  
      ImGui_ColorEdit3("Color", light->light.color, ImGuiColorEditFlags_None);  
      ImGui_DragFloat3("Direction", light->light.direction);
      ImGui_DragFloat("Inner angle", &light->light.cutOff);  
      ImGui_DragFloat("Outer angle", &light->light.outerCutOff);  
    }
    if(node->type == NODE_PLAYER)
    {
      PlayerNode* player = (PlayerNode*)node;
      ImGui_DragFloat("Speed", &player->speed);
      ImGui_DragFloat("Jump force", &player->jumpForce);
      ImGui_DragFloat("Health", &player->health);
      ImGui_Checkbox("Affected by gravity", &player->gravityAffected);
      ImGui_Checkbox("Is active", &player->isActive);
    }
    if(node->type == NODE_COLLISION)
    {
      ColliderNode* collider = (ColliderNode*)node;
      ImGui_DragFloat3("Min AABB", collider->min);
      ImGui_DragFloat3("Max AABB", collider->max);
      ImGui_Checkbox("Is Trigger", &collider->isTrigger);
      ImGui_Checkbox("Is Static", &collider->isStatic);
    }
    scene->lights->lightsDirty = true;
    SpatialNodeUpdateGlobalTransform(spatial);
  }


  ImGui_End();
}
