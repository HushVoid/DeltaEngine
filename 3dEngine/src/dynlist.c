#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynlist.h"



void* dynlistInit(size_t elemSize, size_t initCapacity)
{
  size_t total_size = sizeof(dynlistHeader) + elemSize + initCapacity;
  dynlistHeader* header = (dynlistHeader*)malloc(total_size);
  if(!header)
  {
   printf("dynlist malloc failed");
   return NULL;
  }
  header->size = 0;
  header->capacity = initCapacity;
  header->elemSize = elemSize;

  return DYNLIST_DATA_PTR(header);
  
}

void* dynlistPush(void* userdata, const void* value)
{
   dynlistHeader* header = DYNLIST_HEADER(userdata);
  if(header->size >= header->capacity)
  {
    header->capacity *= 2;
    size_t newTotalSize = sizeof(dynlistHeader) + header->elemSize + header->capacity;

    dynlistHeader * newHeader = realloc(header, newTotalSize);
    if(!newHeader)
    {
      printf("dynlist realloc failed");
      return NULL;
    }
    userdata = DYNLIST_DATA_PTR(header);
  }
  void* dest = (char*)userdata + (header->size * header->elemSize);
  memcpy(dest, value, header->elemSize);
  header->size++;
  
  return userdata;
}

void* dynlistAt(void* userdata,unsigned int index)
{
  dynlistHeader* header = DYNLIST_HEADER(userdata);
   if(index >= header->size)
  {
    printf("index out of range");
    return NULL;
  }
  return (char*)userdata + (index * header->elemSize);

}
size_t dynlistSize(void* userdata)
{
  dynlistHeader* header = DYNLIST_HEADER(userdata);
  return header->size;
}
void dynlistFree(void* userdata)
{
  if(userdata)
  {
    dynlistHeader* header = DYNLIST_HEADER(userdata);
    free(header);
  }
}
