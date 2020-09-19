#ifndef STORAGE_H_INCLUDE
#define STORAGE_H_INCLUDE

#include <dirent.h>

typedef struct
{
   char* fullpath;
   struct dirent *d;
} Song;

Song* getSongs(int* count);

#endif