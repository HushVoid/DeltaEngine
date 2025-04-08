#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynlist.h"



void* dynlistInit(size_t elemSize, size_t initCapacity)
{
  //Counting total size of list
  size_t total_size = sizeof(dynlistHeader) + elemSize + initCapacity;
  //alocating memory of that size
  dynlistHeader* header = (dynlistHeader*)malloc(total_size);
  if(!header)
  {
   printf("dynlist malloc failed");
   return NULL;
  }
  //initializing dynlist properties; 
  header->size = 0;
  header->capacity = initCapacity;
  header->elemSize = elemSize;

  //returning data pointer
  return DYNLIST_DATA_PTR(header);
  
}
void* dynlistPushArray(void* userdata, const void* array, size_t arrayCount)
{
  //finding head pointer from data pointer 
    dynlistHeader* header = DYNLIST_HEADER(userdata);
    
  //counting new size
    size_t newSize = header->size + arrayCount;
  //if size is bigger than capacitiy reallocating the memory and changing pointers acordingly
    if (newSize > header->capacity)
    {
        while (header->capacity < newSize) 
        {
            header->capacity *= 2;
        }
        
        size_t newTotalSize = sizeof(dynlistHeader) + header->elemSize * header->capacity;
        dynlistHeader* newHeader = realloc(header, newTotalSize);
        if (!newHeader) 
        {
            printf("dynlist realloc failed\n");
            return NULL;
        }
        header = newHeader;
        userdata = DYNLIST_DATA_PTR(header);
    }
    
    //Copying array into dynlist;
    void* dest = (char*)userdata + (header->size * header->elemSize);
    memcpy(dest, array, header->elemSize * arrayCount);
    header->size = newSize;
    
    return userdata;
}

void* dynlistPush(void* userdata, const void* value)
{
   return dynlistPushArray(userdata, value, 1);
}

void* dynlistAt(void* userdata,unsigned int index)
{
  //finding header pointer
  dynlistHeader* header = DYNLIST_HEADER(userdata);
  //checking out of range exception
   if(index >= header->size)
  {
    printf("index out of range");
    return NULL;
  }
  // returning element pointer
  return (char*)userdata + (index * header->elemSize);

}
size_t dynlistSize(void* userdata)
{
  //finding header
  dynlistHeader* header = DYNLIST_HEADER(userdata);
  if(header)
  //returning size
    return header->size;
  printf("Header pointer not found");
  return 0;
}
void dynlistFree(void* userdata)
{
  //Cleaning memory
  if(userdata)
  {
    dynlistHeader* header = DYNLIST_HEADER(userdata);
    free(header);
  }
}
