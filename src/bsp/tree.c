#include "bsp/tree.h"
#include "obj/face.h"
#include "obj/obj.h"
#include "obj/vertex_coord.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct BSPTree_s
{
  void *tmp;
};

/**
 *  Given a face `f`, this writes its 3 vertices from `v` into out. This
 *  function assumes out is of length 3.
 */
static int
get_face_verts (_OBJFace_t *f, DynamicArr_t *v, float out[static 1])
{
  if (!f || !v)
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

/**
 *  Calculates a normal, n, of face `f` using its vertices from `v`. Assumes
 *  len of `n` is at least 3.
 */
static int
calc_face_norm (_OBJFace_t *f, DynamicArr_t *v, float n[static 1])
{
  if (!f || !v)
    return -1;

  size_t *idx = objf_get_vert_idxs (f);
  if (!idx)
    return -1;

  float c[3][3] = { 0 };
  for (size_t j = 0; j < 3; j++)
    {
      _OBJVertexCoord_t *curr_v = (_OBJVertexCoord_t *)DynA_at (v, idx[j]);
      if (!curr_v)
        return -1;

      c[j][0] = objv_get_x (curr_v);
      c[j][1] = objv_get_y (curr_v);
      c[j][2] = objv_get_z (curr_v);
    }

  /**
   *  See: https://wikis.khronos.org/opengl/Calculating_a_Surface_Normal.
   */
  float e0[3] = { c[1][0] - c[0][0], c[1][1] - c[0][1], c[1][2] - c[0][2] };
  float e1[3] = { c[2][0] - c[0][0], c[2][1] - c[0][1], c[2][2] - c[0][2] };

  n[0] = (e0[1] * e1[2]) - (e0[2] * e1[1]);
  n[1] = (e0[2] * e1[0]) - (e0[0] * e1[2]);
  n[2] = (e0[0] * e1[1]) - (e0[1] * e1[0]);

  float len = sqrt (n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);

  n[0] /= len;
  n[1] /= len;
  n[2] /= len;

  return 0;
}

static uint32_t
score_split (float o[static 1], float n[static 1], size_t idx, DynamicArr_t *f,
             DynamicArr_t *v)
{
  if (!f || !v)
    return UINT32_MAX;

  int32_t count = 0;
  const size_t N_FACES = DynA_get_size (f);
  for (size_t i = 0; i < N_FACES; i++)
    {
      if (idx == i)
        continue;

      _OBJFace_t *curr = (_OBJFace_t *)DynA_at (f, i);
      if (!curr)
        return UINT32_MAX;

      float end[3] = { 0 };
      if (get_face_verts (curr, v, end) != 0)
        return UINT32_MAX;

      end[0] /= 3;
      end[1] /= 3;
      end[2] /= 3;

      float diff[3] = {
        end[0] - o[0],
        end[1] - o[1],
        end[2] - o[2],
      };

      float dist = n[0] * diff[0] + n[1] * diff[1] + n[2] * diff[2];
      if (dist < 0)
        count -= 1;
      else
        count += 1;
    }

  return (uint32_t)sqrt (count * count);
}

_OBJFace_t *
find_splitting_plane (_OBJObj_t *o)
{
  DynamicArr_t *faces = objo_get_faces (o);
  DynamicArr_t *verts = objo_get_verts (o);
  if ((!faces) || (!verts))
    return NULL;

  _OBJFace_t *out = NULL;
  uint32_t min_count = UINT32_MAX;
  const size_t N_FACES = DynA_get_size (faces);
  for (size_t i = 0; i < N_FACES; i++)
    {
      _OBJFace_t *curr = (_OBJFace_t *)DynA_at (faces, i);
      if (!curr)
        return NULL;

      float norm[3] = { 0 };
      if (calc_face_norm (curr, verts, norm) != 0)
        return NULL;

      float origin[3] = { 0 };
      if (get_face_verts (curr, verts, origin) != 0)
        return NULL;

      origin[0] /= 3;
      origin[1] /= 3;
      origin[2] /= 3;

      uint32_t count = score_split (origin, norm, i, faces, verts);
      if (count < min_count)
        {
          printf ("New min count: %d\n", count);
          printf ("Old: %d\n", min_count);
          printf ("Index: %zu\n", i);
          min_count = count;
          out = curr;
        }
    }

  return out;
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

      _OBJFace_t *splitting_plane = find_splitting_plane (curr);
      if (!splitting_plane)
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
