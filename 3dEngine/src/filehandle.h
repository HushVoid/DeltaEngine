#ifndef FILEREAD_H
#define FILEREAD_H


#include <stdio.h>
#include <stdlib.h>


char *ReadFromFile(const char* filename);
void *WriteToFile(const char* filename, const char* string);

#endif
