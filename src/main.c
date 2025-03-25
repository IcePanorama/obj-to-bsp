/**
 *  NOTE: This whole project is an experiment. Things might look a little rough
 *  while I'm figuring everything out, but I'll come back and clean it all back
 *  up at some later point.
 */
#include "dynamic_arr.h"
#include "log.h"
#include "obj.h"
#include "utils.h"

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_vector_double.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
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
  ObjFile_t obj;
  int status = create_obj_file_from_file (&obj, argv[1]);
  if (status != 0)
    return EXIT_FAILURE;

  float centroid[4] = { 0 };
  calc_centroid_from_obj (centroid, &obj);
  LOG_DEBUG_INFO ("Centroid: (%f, %f, %f, %f)\n", centroid[0], centroid[1],
                  centroid[2], centroid[3]);

  float covar_mat[16] = { 0 };
  calc_covar_mat_from_obj_centroid (&obj, centroid, covar_mat);

  print_covar_mat (covar_mat);

  float eval[4] = { 0 };
  float evec[4][4] = { { 0 } };
  if (calc_eigenvals_vecs (covar_mat, eval, evec) != 0)
    {
      free_obj_file (&obj);
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

  // FIXME: should be checking if front is null here.
  DynamicArray_t *front = dyna_create (1);

  /**
   *  Need to create a dynamic array type and then stuff the remnants into a
   *  bsp interface thingy.
   */
  size_t back_len = 0;
  size_t back_max = 1;
  struct PolygonalFace_s **back
      = calloc (1, sizeof (struct PolygonalFace_s *));
  if (back == NULL)
    {
      dyna_free (front);
      free_obj_file (&obj);
      return EXIT_FAILURE;
    }
  size_t split_len = 0;
  size_t split_max = 1;
  struct PolygonalFace_s **split
      = calloc (1, sizeof (struct PolygonalFace_s *));
  if (split == NULL)
    {
      free (back);
      dyna_free (front);
      free_obj_file (&obj);
      return EXIT_FAILURE;
    }

  for (size_t i = 0; i < obj.num_faces; i++)
    {
      struct PolygonalFace_s *curr = &obj.faces_list[i];
      int8_t cnt = 0;
      for (size_t j = 0; j < 3; j++)
        {
          float tmp[4] = { curr->verts[j]->x, curr->verts[j]->y,
                           curr->verts[j]->z, curr->verts[j]->w };
          float dist = signed_dist (centroid, largest_evec, tmp);

          cnt += dist > 0 ? 1 : -1;
        }

      if (cnt == -3)
        {
          if (back_len == back_max)
            {
              back_max *= 2;
              struct PolygonalFace_s **tmp = realloc (
                  back, back_max * sizeof (struct PolygonalFace_s *));
              if (tmp == NULL)
                {
                  LOG_ERROR ("Failed to resize back list to size %ld.\n",
                             back_max);
                  goto err;
                }

              back = tmp;
            }
          back[back_len] = curr;
          back_len++;
        }
      else if (cnt == 3)
        {
          if (dyna_append (front, curr) != 0)
            {
              break;
            }
        }
      else
        {
          if (split_len == split_max)
            {
              split_max *= 2;
              struct PolygonalFace_s **tmp = realloc (
                  split, split_max * sizeof (struct PolygonalFace_s *));
              if (tmp == NULL)
                {
                  LOG_ERROR ("Failed to resize split list to size %ld.\n",
                             split_max);
                  goto err;
                }

              split = tmp;
            }
          split[split_len] = curr;
          split_len++;
        }
    }

  free (back);
  dyna_free (front);

  printf ("Faces to be split: \n");
  for (size_t i = 0; i < split_len; i++)
    {
      printf ("Face %ld: \n", i);
      struct PolygonalFace_s *curr = split[i];
      for (size_t j = 0; j < 3; j++)
        {
          printf ("%f, %f, %f, %f\n", curr->verts[j]->x, curr->verts[j]->y,
                  curr->verts[j]->z, curr->verts[j]->w);
        }
      puts ("-----------");
    }

  free (split);
  free_obj_file (&obj);
  return EXIT_SUCCESS;
err:
  free (split);
  free (back);
  free (front);
  free_obj_file (&obj);
  return EXIT_FAILURE;
}
