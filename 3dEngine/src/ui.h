#ifndef UI_H
#define UI_H


#include "scene.h"
#include "include/generated/dcimgui.h"



void DrawSceneHierarchy(Scene* scene);
void DrawNodeTree(Node* node, Node* nodeSelected);

void DrawNodeInspector(Node* node);



#endif
