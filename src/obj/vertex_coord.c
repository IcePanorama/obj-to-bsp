#include "obj/vertex_coord.h"
#include "obj/errors.h"

#include <stdlib.h>
#include <string.h>

struct _OBJVertexCoord_s
{
  float x;
  float y;
  float z;
  // float w; // optional, defaults to 1.0.
};

_OBJVertexCoord_t *
objv_alloc (char input[static 1])
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

  _OBJVertexCoord_t *v = calloc (1, sizeof (_OBJVertexCoord_t));
  if (!v)
    {
      WAVOBJ_OOM_ERR ();
      return NULL;
    }

  v->x = vals[0];
  v->y = vals[1];
  v->z = vals[2];

  return v;
}

void
objv_free (_OBJVertexCoord_t *v)
{
  if (!v)
    return;

  free (v);
  v = NULL;
}

size_t
objv_size (void)
{
  return sizeof (_OBJVertexCoord_t);
}
