/**
 *  NOTE: This whole project is an experiment. Things might look a little rough
 *  while I'm figuring everything out, but I'll come back and clean it all back
 *  up at some later point.
 */
#include "dynamic_arr.h"
#include "log.h"
#include "obj/face.h"
#include "obj/file.h"
#include "obj/vertex_coord.h"
#include "utils.h"

#include <stdint.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      LOG_ERROR ("Improper usage error.\n\ttry: %s path/to/file.obj\n",
                 argv[0]);
      return EXIT_FAILURE;
    }

  // FIXME: assuming argv[1] is an obj file for the time being.
  ObjFile_t *obj = obj_create (argv[1]);
  if (obj == NULL)
    return EXIT_FAILURE;

  /**
   *  NOTE: The calculations required to traverse a tree split in this manner
   *  may be too expensive for platforms with constrained hardware. May be
   *  worth exploring other splitting methods in the future.
   *  A cheaper splitting plan
   *  may be to treat the normal of the splitting plane as some fixed value. We
   *  could build a BSP tree for each x-, y-, and z-axis aligned normal (as
   *  well as their negative counterparts), and then select the tree which is
   *  the shortest as our output.
   */
  float centroid[4] = { 0 };
  obj_calc_centroid (obj, centroid);
  LOG_DEBUG_INFO ("Centroid: (%f, %f, %f, %f)\n", centroid[0], centroid[1],
                  centroid[2], centroid[3]);

  float covar_mat[16] = { 0 };
  obj_calc_covar_mat_w_centroid (obj, centroid, covar_mat);

  print_covar_mat (covar_mat);

  float eval[4] = { 0 };
  float evec[4][4] = { { 0 } };
  if (calc_eigenvals_vecs (covar_mat, eval, evec) != 0)
    {
      obj_free (obj);
      return EXIT_FAILURE;
    }

  float *largest_evec;
  get_largest_evec_from_largest_eval (eval, evec, &largest_evec);

  printf ("splitting plane normal: ");
  for (size_t i = 0; i < 4; i++)
    {
      printf ("%f ", largest_evec[i]);
    }
  putchar ('\n');

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
          float dist = signed_dist (centroid, largest_evec, tmp);

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
      printf ("Face %ld: \n", i);
      struct PolygonalFace_s *curr
          = (struct PolygonalFace_s *)dyna_at (split, i);
      if (curr == NULL)
        break;

      for (size_t j = 0; j < 3; j++)
        {
          printf ("%f, %f, %f, %f\n", curr->verts[j]->x, curr->verts[j]->y,
                  curr->verts[j]->z, curr->verts[j]->w);
        }
      puts ("-----------");
    }

  dyna_free (back);
  dyna_free (front);
  dyna_free (split);
  obj_free (obj);
  return EXIT_SUCCESS;
err_exit:
  dyna_free (back);
  dyna_free (front);
  dyna_free (split);
  obj_free (obj);
  return EXIT_FAILURE;
}
