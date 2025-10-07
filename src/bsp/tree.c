#include "bsp/tree.h"
#include "obj/face.h"
#include "obj/obj.h"
#include "obj/vertex_coord.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct BSPTree_s
{
  void *tmp;
};

static int
calc_obj_centroid (_OBJObj_t *o, float *out)
{
  DynamicArr_t *faces = objo_get_faces (o);
  DynamicArr_t *verts = objo_get_verts (o);
  if ((!faces) || (!verts))
    return -1;

  size_t n_faces = DynA_get_size (faces);
  for (size_t i = 0; i < n_faces; i++)
    {
      _OBJFace_t *f = (_OBJFace_t *)DynA_at (faces, i);
      if (!f)
        {
          fprintf (stderr, "%s: Error fetching face at idx %zu", __func__, i);
          return -1;
        }

      size_t *idx = objf_get_vert_idxs (f);

      for (size_t j = 0; j < 3; j++)
        {
          _OBJVertexCoord_t *v = (_OBJVertexCoord_t *)DynA_at (verts, idx[j]);
          if (!v)
            {
              fprintf (stderr,
                       "%s: Error fetching vertex coordinate at idx %zu",
                       __func__, i);
              return -1;
            }

          out[0] += objv_get_x (v);
          out[1] += objv_get_y (v);
          out[2] += objv_get_z (v);
        }
    }

  out[0] /= n_faces * 3;
  out[1] /= n_faces * 3;
  out[2] /= n_faces * 3;

  return 0;
}

float
dist (float x0, float y0, float z0, float x1, float y1, float z1)
{
  return sqrtf (powf (x1 - x0, 2) + powf (y1 - y0, 2) + powf (z1 - z0, 2));
}

_OBJFace_t *
find_centermost_face (_OBJObj_t *o, float c[static 1])
{
  DynamicArr_t *faces = objo_get_faces (o);
  DynamicArr_t *verts = objo_get_verts (o);
  if ((!faces) || (!verts))
    return NULL;

  _OBJFace_t *out = (_OBJFace_t *)DynA_at (faces, 0);
  float out_dist = 0;

  size_t n_faces = DynA_get_size (faces);
  for (size_t i = 0; i < n_faces; i++)
    {
      _OBJFace_t *curr = (_OBJFace_t *)DynA_at (faces, i);
      if (!curr)
        return NULL;

      size_t *idx = objf_get_vert_idxs (curr);
      float f_cent[3] = { 0 };
      for (size_t j = 0; j < 3; j++)
        {
          _OBJVertexCoord_t *v = (_OBJVertexCoord_t *)DynA_at (verts, idx[j]);

          f_cent[0] += objv_get_x (v);
          f_cent[1] += objv_get_y (v);
          f_cent[2] += objv_get_z (v);
        }

      f_cent[0] /= 3;
      f_cent[1] /= 3;
      f_cent[2] /= 3;

      printf ("Face: (%f, %f, %f)\n", f_cent[0], f_cent[1], f_cent[2]);

      float curr_dist
          = dist (c[0], c[1], c[2], f_cent[0], f_cent[1], f_cent[2]);

      printf ("dist: %f\n", curr_dist);
      break;
    }

  return out;
  (void)out_dist;
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
      _OBJObj_t *curr = *(_OBJObj_t **)DynA_at (objs, i);

      float c[3] = { 0 };
      if (calc_obj_centroid (curr, c) != 0)
        return NULL;

      printf ("Centroid: (%f, %f, %f)\n", c[0], c[1], c[2]);

      _OBJFace_t *c_face = find_centermost_face (curr, c);
      if (!c_face)
        return NULL;

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
