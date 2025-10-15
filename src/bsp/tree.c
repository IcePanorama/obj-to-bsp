#include "bsp/tree.h"
#include "obj/face.h"
#include "obj/obj.h"
#include "obj/vertex_coord.h"

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BSPEPS (1e-6)

typedef struct BSPNode_s
{
  float pos[3];
  float norm[3];
  struct BSPNode_s *left;
  struct BSPNode_s *right;
} BSPNode_t;

struct BSPTree_s
{
  DynamicArr_t *head;
};

/**
 *  Given a face `f`, this writes its 3 vertices from `v` into out. This
 *  function assumes out is of length 3.
 */
static int
get_face_centroid (_OBJFace_t *f, DynamicArr_t *v, float out[static 1])
{
  if (!f || !v)
    return -1;

  size_t *idx = objf_get_vert_idxs (f);

  for (size_t j = 0; j < 3; j++)
    {
      _OBJVertexCoord_t *curr = (_OBJVertexCoord_t *)DynA_at (v, idx[j]);
      if (!curr)
        return -1;

      out[0] += objv_get_x (curr);
      out[1] += objv_get_y (curr);
      out[2] += objv_get_z (curr);
    }

  out[0] /= 3;
  out[1] /= 3;
  out[2] /= 3;

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
      _OBJVertexCoord_t *curr = (_OBJVertexCoord_t *)DynA_at (v, idx[j]);
      if (!curr)
        return -1;

      c[j][0] = objv_get_x (curr);
      c[j][1] = objv_get_y (curr);
      c[j][2] = objv_get_z (curr);
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

/** `o` (the output) should be of type float[3]. */
static void
calc_dist (float x0, float y0, float z0, float x1, float y1, float z1,
           float o[static 1])
{
  o[0] = x1 - x0;
  o[1] = y1 - y0;
  o[2] = z1 - z0;
}

// TODO: inline or macro?
static float
sign_dist (float x, float y, float z, float nx, float ny, float nz)
{
  return nx * x + ny * y + nz * z;
}

/**
 *  Scores split based on centroids of faces rather than on individual
 *  vertices.
 */
static uint32_t
score_split_basic (float o[static 1], float n[static 1], size_t idx,
                   DynamicArr_t *f, DynamicArr_t *v)
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
      if (get_face_centroid (curr, v, end) != 0)
        return UINT32_MAX;

      float diff[3] = { 0 };
      calc_dist (o[0], o[1], o[2], end[0], end[1], end[2], diff);
      float sdist = sign_dist (diff[0], diff[1], diff[2], n[0], n[1], n[2]);
      if (sdist < 0)
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
      if (get_face_centroid (curr, verts, origin) != 0)
        return NULL;

      uint32_t count = score_split_basic (origin, norm, i, faces, verts);
      if (count < min_count)
        {
          min_count = count;
          out = curr;
        }
    }

  printf ("Splitting plane preliminary score: %d\n", min_count);
  return out;
}

/** Returns `INFINITY` upon failure. */
static float
get_vertex_orientation (BSPNode_t n[static 1], _OBJVertexCoord_t *v)
{
  if (!v)
    return INFINITY;

  float d[3] = { 0 };
  calc_dist (n->pos[0], n->pos[1], n->pos[2], objv_get_x (v), objv_get_y (v),
             objv_get_z (v), d);

  return sign_dist (d[0], d[1], d[2], n->norm[0], n->norm[1], n->norm[2]);
}

/** Returns value outside the range of [-3, 3] upon failure. */
static int
get_face_orientation (BSPNode_t n[static 1], DynamicArr_t *v, _OBJFace_t *f)
{
  size_t *vert_idx = objf_get_vert_idxs (f);
  if (!vert_idx)
    return INT_MAX;

  int32_t cnt = 0;
  for (size_t i = 0; i < 3; i++)
    {
      _OBJVertexCoord_t *curr = (_OBJVertexCoord_t *)DynA_at (v, vert_idx[i]);
      if (!curr)
        return INT_MAX;

      float orientation = get_vertex_orientation (n, curr);
      if (fabs (orientation - INFINITY) < (BSPEPS))
        return INT_MAX; // get_vertex_orientation err
      else if (orientation > (BSPEPS))
        cnt += 1;
      else if (orientation < -(BSPEPS))
        cnt -= 1;
      else
        cnt += 0;
      printf ("cnt: %d\n", cnt);
    }

  return cnt;
}

BSPTree_t *
bsp_alloc (OBJFile_t *o)
{
  DynamicArr_t *objs = obj_get_objs (o);
  if (!objs)
    return NULL;

  const size_t N_OBJS = DynA_get_size (objs);
  for (size_t i = 0; i < N_OBJS; i++)
    {
      _OBJObj_t *curr = *(_OBJObj_t **)DynA_at (objs, i);

      _OBJFace_t *splitting_plane = find_splitting_plane (curr);
      if (!splitting_plane)
        return NULL;

      DynamicArr_t *faces = objo_get_faces (curr);
      DynamicArr_t *verts = objo_get_verts (curr);
      if ((!faces) || (!verts))
        return NULL;

      BSPNode_t n = { 0 };
      if ((calc_face_norm (splitting_plane, verts, n.norm) != 0)
          || (get_face_centroid (splitting_plane, verts, n.pos) != 0))
        return NULL;

      DynamicArr_t *in_front = DynA_alloc (sizeof (_OBJFace_t *));
      DynamicArr_t *behind = DynA_alloc (sizeof (_OBJFace_t *));
      DynamicArr_t *to_split = DynA_alloc (sizeof (_OBJFace_t *));
      if ((!in_front) || (!behind) || (!to_split))
        {
          DynA_free (in_front);
          DynA_free (behind);
          DynA_free (to_split);
          return NULL;
        }

      const size_t N_FACES = DynA_get_size (faces);
      for (size_t j = 0; j < N_FACES; j++)
        {
          _OBJFace_t *tmp = (_OBJFace_t *)DynA_at (faces, j);
          if (!tmp)
            {
              DynA_free (in_front);
              DynA_free (behind);
              DynA_free (to_split);
              return NULL;
            }

          int orientation = get_face_orientation (&n, verts, tmp);
          if ((orientation < -3) || (3 < orientation))
            {
              DynA_free (in_front);
              DynA_free (behind);
              DynA_free (to_split);
              return NULL;
            }

          if (orientation == 3)
            {
              if (DynA_append (in_front, (void **)&tmp) != 0)
                {
                  DynA_free (in_front);
                  DynA_free (behind);
                  DynA_free (to_split);
                  return NULL;
                }
            }
          else if (orientation == -3)
            {
              if (DynA_append (behind, (void **)&tmp) != 0)
                {
                  DynA_free (in_front);
                  DynA_free (behind);
                  DynA_free (to_split);
                  return NULL;
                }
            }
          else
            {
              if (DynA_append (to_split, (void **)&tmp) != 0)
                {
                  DynA_free (in_front);
                  DynA_free (behind);
                  DynA_free (to_split);
                  return NULL;
                }
            }
        }

      printf ("in_front size: %zu\n", DynA_get_size (in_front));
      printf ("behind size: %zu\n", DynA_get_size (behind));
      printf ("to_split size: %zu\n", DynA_get_size (to_split));

      printf ("\nn pos: %f, %f, %f\n", n.pos[0], n.pos[1], n.pos[2]);
      printf ("n norm: %f, %f, %f\n\n", n.norm[0], n.norm[1], n.norm[2]);

      printf ("Eps: %f\n", (BSPEPS));
      const size_t N_SPLITS = DynA_get_size (to_split);
      for (size_t j = 0; j < N_SPLITS; j++)
        {
          // FIXME: not checking if these are null!
          _OBJFace_t *f = *(_OBJFace_t **)DynA_at (to_split, j);
          size_t *idx = objf_get_vert_idxs (f);
          printf ("%d\n", get_face_orientation (&n, verts, f));
          for (size_t k = 0; k < 3; k++)
            {
              _OBJVertexCoord_t *v = DynA_at (verts, idx[k]);
              printf ("f: %f, %f, %f\n", objv_get_x (v), objv_get_y (v),
                      objv_get_z (v));
              printf ("%f\n", get_vertex_orientation (&n, v));
            }
          printf ("------------\n");
          break;
        }

      DynA_free (in_front);
      DynA_free (behind);
      DynA_free (to_split);
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
