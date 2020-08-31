#include <stdio.h>
#include "storage.h"

void app_main(void)
{
  int songsCount;
  Song *songs = getSongs(&songsCount);

  for (int i = 0; i < songsCount; i++)
    printf("song: %s\n", (songs + i)->d->d_name);
}
