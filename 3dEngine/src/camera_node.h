#ifndef CAMERA_NODE_H
#define CAMERA_NODE_H

#include "spatial_node.h"
#include "camera.h"

typedef struct 
{
  SpatialNode base;
  Camera camera;
} CameraNode;

#endif // !CAMERA_NODE_H
