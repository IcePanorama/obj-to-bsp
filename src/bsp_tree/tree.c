#include "bsp_tree/tree.h"
#include "bsp_tree/node.h"
#include "dynamic_arr.h"
#include "log.h"

#include <stdlib.h>

/*
 *  TODO: when we serialize this, we need to export the size and capacity of
 *  the dynamic array so that users of our bsp trees don't need to have this
 *  DynamicArray type defined.
 */
struct BSPTree_s
{
  enum _PlaneNormal_e
  {
    PN_X_ALIGNED,
    PN_NEG_X_ALIGNED,
    PN_Y_ALIGNED,
    PN_NEG_Y_ALIGNED,
    PN_Z_ALIGNED,
    PN_NEG_Z_ALIGNED
  } normal;
  DynamicArray_t *nodes;
};

BSPTree_t *
bsp_create (void)
{
  BSPTree_t *out = calloc (1, sizeof (BSPTree_t));
  if (out == NULL)
    goto out_of_mem_err;

  out->nodes = dyna_create (sizeof (BSPNode_t));
  if (out->nodes == NULL)
    {
      free (out);
      goto out_of_mem_err;
    }

  return out;
out_of_mem_err:
  LOG_ERROR_MSG ("Out of memory error: couldn't allocate memory for BSPTree.");
  return NULL;
}

void
bsp_free (BSPTree_t *t)
{
  if (t == NULL)
    return;

  if (t->nodes != NULL)
    dyna_free (t->nodes);

  free (t);
  t = NULL;
}
