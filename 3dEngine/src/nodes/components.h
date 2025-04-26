#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "../dynlist.h"
#include <stdbool.h>
#include "../include/cglm/cglm.h"
#include "../model.h"
#include "../mesh.h"
#include "../include/cJSON/cJSON.h"

typedef struct 
{
  vec3 position;
  vec3 scale;
  vec3 rotation;
} Transform;



void TransformDefaultInit(Transform *t);
void TransformCopyTo(Transform *src, Transform *dest);

void TransformGetForward(const Transform* t, vec3 dest);
void TransformGetForwardNoPitch(const Transform* t, vec3 dest);
void TransformGetRightVec(const Transform *t, vec3 worldUp, vec3 dest);

cJSON* TransformToJSON(const Transform* t);



#endif // !COMPONENTS_H
