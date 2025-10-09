#include "bsp/tree.h"
#include "obj/face.h"
#include "obj/obj.h"
#include "obj/vertex_coord.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct BSPTree_s
{
  void *tmp;
};

static int
get_face_verts (_OBJFace_t *f, DynamicArr_t *v, float *out)
{
  if (!f)
    return -1;

  size_t *idx = objf_get_vert_idxs (f);

  for (size_t j = 0; j < 3; j++)
    {
      _OBJVertexCoord_t *curr_v = (_OBJVertexCoord_t *)DynA_at (v, idx[j]);
      if (!curr_v)
        return -1;

      out[0] += objv_get_x (curr_v);
      out[1] += objv_get_y (curr_v);
      out[2] += objv_get_z (curr_v);
    }

  return 0;
}

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

      float f_cent[3] = { 0 };
      if (get_face_verts (f, verts, f_cent) != 0)
        {
          fprintf (stderr, "%s: error calculating centroid of face #%zu\n",
                   __func__, i);
          return -1;
        }

      out[0] += f_cent[0];
      out[1] += f_cent[1];
      out[2] += f_cent[2];
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

static int
calc_face_norm (_OBJFace_t *f, DynamicArr_t *v, float n[static 1])
{
  if (!f || !v)
    return -1;

  size_t *idx = objf_get_vert_idxs (f);
  if (!idx)
    return -1;

  float fverts[3][3] = { 0 };
  for (size_t j = 0; j < 3; j++)
    {
      _OBJVertexCoord_t *curr_v = (_OBJVertexCoord_t *)DynA_at (v, idx[j]);
      if (!curr_v)
        return -1;

      fverts[j][0] = objv_get_x (curr_v);
      fverts[j][1] = objv_get_y (curr_v);
      fverts[j][2] = objv_get_z (curr_v);
    }

  /**
   *  See: https://wikis.khronos.org/opengl/Calculating_a_Surface_Normal.
   */
  float u[3] = { fverts[1][0] - fverts[0][0], fverts[1][1] - fverts[0][1],
                 fverts[1][2] - fverts[0][2] };

  float _v[3] = { fverts[2][0] - fverts[0][0], fverts[2][1] - fverts[0][1],
                  fverts[2][2] - fverts[0][2] };

  n[0] = (u[1] * _v[2]) - (u[2] * _v[1]);
  n[1] = (u[2] * _v[0]) - (u[0] * _v[2]);
  n[2] = (u[0] * _v[1]) - (u[1] * _v[0]);

  float len = sqrt (n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);

  n[0] /= len;
  n[1] /= len;
  n[2] /= len;

  return 0;
}

// FIXME: need to rename this func
_OBJFace_t *
find_centermost_face (_OBJObj_t *o, float c[static 1])
{
  DynamicArr_t *faces = objo_get_faces (o);
  DynamicArr_t *verts = objo_get_verts (o);
  if ((!faces) || (!verts))
    return NULL;

  _OBJFace_t *out = NULL;
  const size_t N_FACES = DynA_get_size (faces);
  for (size_t i = 0; i < N_FACES; i++)
    {
      _OBJFace_t *curr = (_OBJFace_t *)DynA_at (faces, i);
      if (!curr)
        return NULL;

      float norm[3] = { 0 };
      if (calc_face_norm (curr, verts, norm) != 0)
        return NULL;

      printf ("Norm: %f, %f, %f\n", norm[0], norm[1], norm[2]);
      break;
    }

  return out;
  (void)c;
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

      //printf ("Obj centroid: (%f, %f, %f)\n", c[0], c[1], c[2]);

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
