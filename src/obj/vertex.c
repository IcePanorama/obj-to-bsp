#include "obj/vertex.h"

#include <stdlib.h>
#include <string.h>

int
objv_init (_OBJVertexCoord_t v[static 1], char input[static 1])
{
  float vals[3] = { 0 };
  size_t i = 0;
  for (char *ptr = input; ptr && i < 3; i++)
    {
      char *end = strchr (ptr, ' ');
      if (end)
        *end = '\0';
      vals[i] = atof (ptr);
      if (end)
        ptr = end + 1;
    }

  v->x = vals[0];
  v->y = vals[1];
  v->z = vals[2];

  return 0;
}
