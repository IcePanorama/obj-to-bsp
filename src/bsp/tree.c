#include "bsp/tree.h"

#include <stdlib.h>

struct BSPTree_s
{
  void *tmp;
};

BSPTree_t *
bsp_alloc (ObjFile_t *o)
{
  return NULL;
  (void)o;
}

void
bsp_free (BSPTree_t *t)
{
  if (!t)
    return;

  free (t);
  t = NULL;
}
