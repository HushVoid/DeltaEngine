#ifndef DYNLIST_H
#define DYNLIST_H

#define DEFAULT_INIT_CAPACITY 16


#include <stdio.h>
#include <stdlib.h>



typedef struct
{
  size_t size;
  size_t elemSize;
  size_t capacity;
  void *items;
}dynlist_t;


//FOR INTERNAL USE ONLY!


dynlist_t*  dynlistInit(size_t elemSize, size_t capacity);
int dynlistPush(dynlist_t *list, const void* value);
int dynlistPushList(dynlist_t *list, dynlist_t* src);
void* dynlistAt(dynlist_t *list,unsigned int index);
void dynlistFree(dynlist_t *list);
int dynlistFreeContainerOnly(dynlist_t* list);

#endif
