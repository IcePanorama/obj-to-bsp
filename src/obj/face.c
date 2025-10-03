#include "obj/face.h"

#include <stdlib.h>

struct _OBJFace_s
{
  size_t vertIdx[3];
};

_OBJFace_t *
objf_alloc (void)
{
  return calloc (1, sizeof (_OBJFace_t));
}

void
objf_free (_OBJFace_t *f)
{
  if (!f)
    return;

  free (f);
  f = NULL;
}

/** Returns the size of `_OBJFace_t` in bytes. */
size_t
objf_size (void)
{
  return sizeof (_OBJFace_t);
}
