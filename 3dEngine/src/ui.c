#include "ui.h"


void DrawSceneHierarchy(Scene* scene)
{
  ImGui_Begin("Scene hierarchy", NULL, ImGuiWindowFlags_None);
  DrawNodeTree(scene->root, NULL);

  if(ImGui_BeginPopupContextWindow())
  {
    if(ImGui_MenuItem("Create base"))
    {
      Node* newNode = NodeCreate("New node");
      NodeAddChild(scene->root, newNode);
    }
    if(ImGui_MenuItem("Create spatial"))
    {
      SpatialNode* newNode = SpatialNodeCreate("New spatial");
      NodeAddChild(scene->root, (Node*)newNode);
    }
    if(ImGui_MenuItem("Create Directional light"))
    {
      DirectionalLightNode* newNode = DLightCreateDefault("Dir light");
      NodeAddChild(scene->root, (Node*)newNode);
    }
    if(ImGui_MenuItem("Create Point light"))
    {
      PointLightNode* newNode = PLightCreateDefault("Point light");
      NodeAddChild(scene->root, (Node*)newNode);
    }
    if(ImGui_MenuItem("Create Spot light"))
    {
      SpotLightNode* newNode = SLightCreateDefault("Spot light");
      NodeAddChild(scene->root, (Node*)newNode);
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
    if(ImGui_MenuItem("Custom model..."))
    {
      ImGui_OpenPopup("Custom Model Path", ImGuiPopupFlags_None);
    }
    ImGui_EndPopup();
  }
  if(ImGui_BeginPopupModal("Custom Model Path", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
void DrawNodeTree(Node* node, Node* nodeSelected)
{
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
  if(node == nodeSelected) flags |= ImGuiTreeNodeFlags_Selected;
  if(node->children == NULL || node->children->size == 0) flags |= ImGuiTreeNodeFlags_Leaf;

  bool isOpen = ImGui_TreeNodeExPtr(node, flags, "%s (%s)", node->name, NodeT2Str(node->type));
  if(ImGui_IsItemClicked())
  {
    nodeSelected = node; 
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
      if(draggedNode != node)
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
        DrawNodeTree(child, nodeSelected);
      }
    }
    ImGui_TreePop();
  }
}
void DrawNodeInspector(Node* node)
{
  
}
