#include <dirent.h>

typedef struct
{
   char* fullpath;
   struct dirent *d;
} Song;