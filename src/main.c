#include "log.h"
#include "obj.h"
#include "utils.h"

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_vector_double.h>
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

  struct VertexCoord_s centroid = calc_centroid_from_obj (&obj);
  LOG_DEBUG_INFO ("Centroid: (%f, %f, %f, %f)\n", centroid.x, centroid.y,
                  centroid.z, centroid.w);

  float covar_mat[16] = { 0 };
  calc_covar_mat_from_obj_centroid (&obj, &centroid, covar_mat);
  free_obj_file (&obj);

  print_covar_mat (covar_mat);

  float eval[4] = { 0 };
  float evec[4][4] = { { 0 } };
  if (calc_eigenvals_vecs (covar_mat, eval, evec) != 0)
    {
      return EXIT_FAILURE;
    }

  puts ("Eigenvalues:");
  for (size_t i = 0; i < 4; i++)
    {
      printf ("%2.4f ", eval[i]);
    }
  putchar ('\n');

  puts ("Eigenvectors:");
  for (size_t i = 0; i < 4; i++)
    {
      for (size_t j = 0; j < 4; j++)
        {
          printf ("%2.4f ", evec[i][j]);
        }
      putchar ('\n');
    }

  return EXIT_SUCCESS;
}
