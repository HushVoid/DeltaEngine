#ifndef DYNLIST_H
#define DYNLIST_H



#define DYNLIST_DATA_PTR(header_ptr) \
    ((void*)((char*)(header_ptr) + sizeof(dynlistHeader)))

#define DYNLIST_HEADER(data_ptr) \
    ((dynlistHeader*)((char*)(data_ptr) - sizeof(dynlistHeader)))
#include <stdio.h>
#include <stdlib.h>


typedef struct
{
  size_t size;
  size_t capacity;
  size_t elemSize;
  void *userdata;
}dynlistHeader;


void* dynlistInit(size_t elemSize, size_t initCapacity);
void* dynlistPush(void* userdata, const void* value);
void* dynlistAt(void* userdata,unsigned int index);
size_t dynlistSize(void* userdata);
void dynlistFree(void* userdata);


#endif
