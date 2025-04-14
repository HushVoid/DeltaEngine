#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynlist.h"



dynlist_t*  dynlistInit(size_t elemSize, size_t capacity)
{
  dynlist_t* list = calloc(1, sizeof(dynlist_t));
  list->elemSize = elemSize;
  list->size = 0;
  list->capacity = capacity;
  list->items = malloc(elemSize * capacity);

  return list;
}

int dynlistPushList(dynlist_t* dest, dynlist_t* src) {
    if (!dest || !src || src->elemSize != dest->elemSize) 
        return -1;

    // Ensure enough capacity
    size_t required = dest->size + src->size;
    if (required > dest->capacity) {
        size_t newCapacity = required * 2;  // Standard growth factor
        void* newItems = realloc(dest->items, dest->elemSize * newCapacity);
        
        if (!newItems) return -2;
        
        dest->items = newItems;
        dest->capacity = newCapacity;
    }

    // Copy all elements at once
    void* destPtr = (char*)dest->items + (dest->size * dest->elemSize);
    memcpy(destPtr, src->items, src->size * src->elemSize);
    dest->size += src->size;
    
    return 0;
}
int dynlistPush(dynlist_t* list, const void* value)
{
    if (!list || !value) return -1;

    // Resize if needed (typically double capacity)
    if (list->size >= list->capacity) {
        size_t newCapacity = list->capacity * 2;
        void* newItems = realloc(list->items, list->elemSize * newCapacity);
        
        if (!newItems) return -2;
        
        list->items = newItems;
        list->capacity = newCapacity;
    }

    // Calculate position and copy element
    void* dest = (char*)list->items + (list->size * list->elemSize);
    memcpy(dest, value, list->elemSize);
    list->size++;
    
    return 0;
}

void* dynlistAt(dynlist_t* list, unsigned int index) {
    if (!list || index >= list->size) return NULL;
    return (char*)list->items + (index * list->elemSize);
}
void dynlistFree(dynlist_t* list)
{
    if (!list) return;
    free(list->items);  // Free the contiguous block
    free(list);        // Free the container
}

int dynlistFreeContainerOnly(dynlist_t* list)
{
    if(!list) 
    {
        printf("dynlistFreeContainerOnly: invalid dynlist\n");
        return -1;
    }
    
    // Only free the container structure itself
    // WITHOUT freeing the individual items or items array
    free(list);
    return 0;
}
int dynlistPop(dynlist_t *list)
{
  dynlistDeleteAt(list, list->size - 1);
  return 0;
}
int dynlistDeleteAt(dynlist_t *list,unsigned int index)
{
  if(!list)
  {
    printf("dynlistDeleteAt: invalid dynlist\n");
    return -1;
  }
  if(index >= list->size)
  {
    printf("dynlistDeleteAt: index out of range");
    return -2;
  }
  void* item = (char*)list->items + index * list->elemSize;
  void* src = (char*)item + list->elemSize;
  size_t bytesToMove = (list->size - index - 1) * list->elemSize;
  if(bytesToMove > 0)
  {
    memmove(item, src, bytesToMove);
  }
  list->size--;
  if(list->size * 4 < list->capacity && list->capacity > DEFAULT_INIT_CAPACITY)
  {
    size_t newCapacity = list->capacity / 2;
    void* newItems = realloc(list->items, newCapacity * list->elemSize);
    if(!newItems)
    {
      printf("dynlistDeleteAt: can not reallocate memory\n");
      return -3;
    }
    list->items = newItems;
    list->capacity = newCapacity;
  }
  return 0;
}
