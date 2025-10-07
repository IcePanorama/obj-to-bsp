#include "bsp/tree.h"
#include "obj/obj.h"

#include <stdio.h>
#include <stdlib.h>

struct BSPTree_s
{
  void *tmp;
};

static int
calc_centroid (_OBJObj_t *o, float *out)
{
  DynamicArr_t *faces = objo_get_faces (o);
  if (!faces)
    return -1;

  printf ("%zu\n", DynA_get_size (faces));
  return 0;
  (void)out;
}

BSPTree_t *
bsp_alloc (OBJFile_t *o)
{
  DynamicArr_t *objs = obj_get_objs (o);
  if (!objs)
    return NULL;

  size_t n_objs = DynA_get_size (objs);
  for (size_t i = 0; i < n_objs; i++)
    {
      float tmp[3] = { 0 };
      _OBJObj_t *curr = *(_OBJObj_t **)DynA_at (objs, 0);
      calc_centroid (curr, tmp);
      break;
    }

  return NULL;
}

void
bsp_free (BSPTree_t *t)
{
  if (!t)
    return;

  free (t);
  t = NULL;
}
