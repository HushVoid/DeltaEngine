#ifndef UI_H
#define UI_H


#include "scene.h"
#include "include/generated/dcimgui.h"



void DrawSceneHierarchy(Scene* scene, Node** forSelection);
void DrawNodeTree(Node* node, Node** nodeSelected);
void DrawSceneInspector(Scene** scene);
void DrawNodeInspector(Node* node);



#endif
