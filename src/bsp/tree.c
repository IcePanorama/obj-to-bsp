#include "bsp/tree.h"
#include "obj/face.h"
#include "obj/obj.h"
#include "obj/vertex.h"

#include <assert.h>
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
get_face_centroid (OBJFace_t f[static 1], float out[static 1])
{
  for (size_t j = 0; j < 3; j++)
    {
      out[0] += f->vertices[j][0];
      out[1] += f->vertices[j][1];
      out[2] += f->vertices[j][2];
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
calc_face_norm (OBJFace_t f[static 1], float n[static 1])
{
  float *c[3] = { f->vertices[0], f->vertices[1], f->vertices[2] };

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
                   DynamicArr_t *f)
{
  if (!f)
    return UINT32_MAX;

  int32_t count = 0;
  const size_t N_FACES = DynA_get_size (f);
  for (size_t i = 0; i < N_FACES; i++)
    {
      if (idx == i)
        continue;

      OBJFace_t *curr = (OBJFace_t *)DynA_at (f, i);
      if (!curr)
        return UINT32_MAX;

      float end[3] = { 0 };
      if (get_face_centroid (curr, end) != 0)
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

OBJFace_t *
find_splitting_plane (OBJObj_t *o)
{
  DynamicArr_t *faces = objo_get_faces (o);
  if (!faces)
    return NULL;

  OBJFace_t *out = NULL;
  uint32_t min_count = UINT32_MAX;
  const size_t N_FACES = DynA_get_size (faces);
  for (size_t i = 0; i < N_FACES; i++)
    {
      OBJFace_t *curr = (OBJFace_t *)DynA_at (faces, i);
      if (!curr)
        return NULL;

      float norm[3] = { 0 };
      if (calc_face_norm (curr, norm) != 0)
        {
          fprintf (stderr, "[%s] Failed to calculate face norm.\n", __func__);
          return NULL;
        }

      float origin[3] = { 0 };
      if (get_face_centroid (curr, origin) != 0)
        {
          fprintf (stderr, "[%s] Failed to find face centroid.\n", __func__);
          return NULL;
        }

      uint32_t count = score_split_basic (origin, norm, i, faces);
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
get_vertex_orientation (BSPNode_t n[static 1], float v[3])
{
  float d[3] = { 0 };
  calc_dist (n->pos[0], n->pos[1], n->pos[2], v[0], v[1], v[2], d);

  return sign_dist (d[0], d[1], d[2], n->norm[0], n->norm[1], n->norm[2]);
}

/** Returns value outside the range of [-3, 3] upon failure. */
static int
get_face_orientation (BSPNode_t n[static 1], OBJFace_t f[static 1])
{
  int32_t cnt = 0;
  for (size_t i = 0; i < 3; i++)
    {
      float orientation = get_vertex_orientation (n, f->vertices[i]);
      if (orientation > (BSPEPS))
        {
          cnt += 1;
        }
      else if (orientation < -(BSPEPS))
        {
          cnt -= 1;
        }
      else
        {
          cnt += 0;
        }
    }

  assert (-3 <= cnt && cnt <= 3);
  return cnt;
}

int
split_faces (BSPNode_t n[static 1], DynamicArr_t *f, DynamicArr_t *in_front,
             DynamicArr_t *behind, DynamicArr_t *to_split)
{
  const size_t N_FACES = DynA_get_size (f);
  for (size_t j = 0; j < N_FACES; j++)
    {
      OBJFace_t *tmp = (OBJFace_t *)DynA_at (f, j);
      if (!tmp)
        return -1;

      int orientation = get_face_orientation (n, tmp);
      if ((0 <= orientation) && (orientation <= 3))
        {
          if (DynA_append (in_front, (void **)&tmp) != 0)
            return -1;
        }
      else if ((-3 <= orientation) && (orientation < 0))
        {
          if (DynA_append (behind, (void **)&tmp) != 0)
            return -1;
        }
      else
        {
          if (DynA_append (to_split, (void **)&tmp) != 0)
            return -1;
        }
    }

  return 0;
}

// FIXME: should only operate on a single object?, refactor
BSPTree_t *
bsp_alloc (OBJFile_t *o)
{
  DynamicArr_t *objs = obj_get_objs (o);
  if (!objs)
    return NULL;

  const size_t N_OBJS = DynA_get_size (objs);
  for (size_t i = 0; i < N_OBJS; i++)
    {
      OBJObj_t *curr = *(OBJObj_t **)DynA_at (objs, i);

      OBJFace_t *splitting_plane = find_splitting_plane (curr);
      if (!splitting_plane)
        {
          fprintf (stderr, "[%s] Failed to find splitting plane.\n", __func__);
          return NULL;
        }

      BSPNode_t n = { 0 };
      if ((calc_face_norm (splitting_plane, n.norm) != 0)
          || (get_face_centroid (splitting_plane, n.pos) != 0))
        {
          fprintf (stderr,
                   "[%s] Failed to calculate norm or centroid of splitting "
                   "plane.\n",
                   __func__);
          return NULL;
        }

      DynamicArr_t *faces = objo_get_faces (curr);
      if (!faces)
        {
          fprintf (stderr, "[%s] Faces not found for current object.\n",
                   __func__);
          return NULL;
        }

      const size_t FACE_PTR_SIZE = sizeof (OBJFace_t *);
      DynamicArr_t *in_front = DynA_alloc (FACE_PTR_SIZE);
      DynamicArr_t *behind = DynA_alloc (FACE_PTR_SIZE);
      DynamicArr_t *to_split = DynA_alloc (FACE_PTR_SIZE);
      if ((!in_front) || (!behind) || (!to_split))
        {
          fprintf (stderr, "[%s] Out of memory error.\n", __func__);
          DynA_free (in_front);
          DynA_free (behind);
          DynA_free (to_split);
          return NULL;
        }

      if (split_faces (&n, faces, in_front, behind, to_split) != 0)
        {
          fprintf (stderr, "[%s] Failed to split faces.\n", __func__);
          DynA_free (in_front);
          DynA_free (behind);
          DynA_free (to_split);
          return NULL;
        }

      printf ("in_front size: %zu\n", DynA_get_size (in_front));
      printf ("behind size: %zu\n", DynA_get_size (behind));
      printf ("to_split size: %zu\n", DynA_get_size (to_split));
      // fixme later
      assert (DynA_get_size (to_split) == 0);

      printf ("\nn pos: %f, %f, %f\n", n.pos[0], n.pos[1], n.pos[2]);
      printf ("n norm: %f, %f, %f\n", n.norm[0], n.norm[1], n.norm[2]);

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
