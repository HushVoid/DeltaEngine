#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "dynlist.h"
#include <stdbool.h>
#include "include/cglm/cglm.h"
#include "model.h"
#include "mesh.h"

typedef struct 
{
  vec3 position;
  vec3 scale;
  vec3 rotation;
  mat4 localMatrix;
} Transform;


void TransformUpdateMatrix(Transform* t);
/*{
    glm_mat4_identity(t->localMatrix);
    glm_translate(t->localMatrix, t->position);
    glm_quat_rotate(t->localMatrix, t->rotation, t->localMatrix);
    glm_scale(t->localMatrix, t->scale);
}*/





#endif // !COMPONENTS_H
