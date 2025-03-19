#include "log.h"
#include "obj.h"
#include "utils.h"

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_vector_double.h>
#include <stddef.h>
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

  puts ("largest evec:");
  for (size_t i = 0; i < 4; i++)
    {
      printf ("%f ", largest_evec[i]);
    }
  putchar ('\n');

  for (size_t i = 0; i < obj.num_verticies; i++)
    {
      for (size_t j = 0; j < 4; j++)
        {
          printf ("%f ", largest_evec[j]);
        }
      putchar ('\n');

      float tmp[4] = { obj.verticies_list[i].x, obj.verticies_list[i].y,
                       obj.verticies_list[i].z, obj.verticies_list[i].w };
      float dist = signed_dist (centroid, largest_evec, tmp);
      printf ("dist: %f\n", dist);
    }

  free_obj_file (&obj);
  return EXIT_SUCCESS;
}
