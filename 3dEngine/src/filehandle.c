#include <stdio.h>
#include <stdlib.h>

char *ReadFromFile(const char* filename)
{
  FILE *file;
  fopen_s(&file, filename, "r");
  if(file == NULL) return NULL;

  fseek(file,0, SEEK_END);
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *string = malloc(sizeof(char) * (length + 1));
  
  char c;
  int i = 0;
  while((c = fgetc(file)) != EOF )
  {
    string[i] = c;
    i++;
  }
  string[i] = '\0';
  fclose(file);
  return string;
}

void WriteToFile(const char* filename, const char* string)
{
  FILE *file;
  fopen_s(&file, filename, "w");
  fprintf(file, "%s", string);
  fclose(file);
}
