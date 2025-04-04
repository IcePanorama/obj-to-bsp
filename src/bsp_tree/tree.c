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
  /*
  enum _PlaneNormal_e
  {
    PN_X_ALIGNED,
    PN_NEG_X_ALIGNED,
    PN_Y_ALIGNED,
    PN_NEG_Y_ALIGNED,
    PN_Z_ALIGNED,
    PN_NEG_Z_ALIGNED,
  } normal;
  */
  DynamicArray_t *nodes; // i = node, 2*i+1 = left child, 2*i+2 = right child
};

BSPTree_t *
bsp_alloc (void)
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

#include "obj/face.h"
#include "obj/vertex_coord.h"
#include "utils.h"

#include <stdint.h>

#define EPSILON (1e-6)

int
bsp_process_obj (BSPTree_t *t, ObjFile_t *obj, float sp_center[static 4],
                 float sp_norm[static 4])
{
  if ((t == NULL) || (obj == NULL))
    return -1;

  DynamicArray_t *front = dyna_create (sizeof (struct PolygonalFace_s));
  DynamicArray_t *back = dyna_create (sizeof (struct PolygonalFace_s));
  DynamicArray_t *split = dyna_create (sizeof (struct PolygonalFace_s));
  if ((front == NULL) || (back == NULL) || (split == NULL))
    goto err_exit;

  DynamicArray_t *faces_list = obj_get_faces_list (obj);
  if (faces_list == NULL)
    goto err_exit;

  for (size_t i = 0; i < dyna_get_size (faces_list); i++)
    {
      struct PolygonalFace_s *curr
          = (struct PolygonalFace_s *)dyna_at (faces_list, i);
      int8_t cnt = 0;
      for (size_t j = 0; j < 3; j++)
        {
          float tmp[4] = { curr->verts[j]->x, curr->verts[j]->y,
                           curr->verts[j]->z, curr->verts[j]->w };
          float dist = signed_dist (sp_center, sp_norm, tmp);

          cnt += dist > 0 ? 1 : -1;
        }

      if (cnt == -3) // behind splitting plane
        {
          if (dyna_append (back, curr) != 0)
            break;
        }
      else if (cnt == 3) // in front of splitting plane
        {
          if (dyna_append (front, curr) != 0)
            break;
        }
      else // needs to be split!
        {
          if (dyna_append (split, curr) != 0)
            break;
        }
    }

  printf ("Faces to be split: \n");
  for (size_t i = 0; i < dyna_get_size (split); i++)
    {
      struct PolygonalFace_s *curr
          = (struct PolygonalFace_s *)dyna_at (split, i);
      if (curr == NULL)
        break;

      /* clang-format off */
      struct OBJVertexCoord_s *edges[3][2] =
      {
        { curr->verts[0], curr->verts[1] },
        { curr->verts[1], curr->verts[2] },
        { curr->verts[2], curr->verts[0] }
      };
      /* clang-format on */

      for (size_t j = 0; j < 3; j++)
        {
          float a[4] = { edges[j][0]->x, edges[j][0]->y, edges[j][0]->z,
                         edges[j][0]->w };
          float b[4] = { edges[j][1]->x, edges[j][1]->y, edges[j][1]->z,
                         edges[j][1]->w };

          float param
              = u_calc_line_plane_intersection (a, b, sp_center, sp_norm);
          printf ("%f\n", param);
          if ((0.0 - (EPSILON)) <= param && param <= (1.0 + (EPSILON)))
            {
              // TODO: calc "v_i + param (v_j - v_i)". See notes.
              puts ("foo");
            }
        }
      break;
    }
  dyna_free (split);

  dyna_free (back);
  dyna_free (front);
  return 0;
err_exit:
  dyna_free (back);
  dyna_free (front);
  dyna_free (split);
  return -1;
}
