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
    goto prg_exit;

  struct VertexCoord_s centroid = calc_centroid_from_obj (&obj);
  LOG_DEBUG_INFO ("Centroid: (%f, %f, %f, %f)\n", centroid.x, centroid.y,
                  centroid.z, centroid.w);

  float covar_mat[16] = { 0 };
  calc_covar_mat_from_obj_centroid (&obj, &centroid, covar_mat);
  print_covar_mat (covar_mat);

  // FIXME: clean up all this garbage:
  gsl_matrix *mat = gsl_matrix_alloc (4, 4);
  status = (mat != NULL) ? 0 : -1;
  if (status != 0)
    goto prg_exit;

  for (size_t i = 0; i < 4; i++)
    {
      for (size_t j = 0; j < 4; j++)
        {
          gsl_matrix_set (mat, i, j, covar_mat[i * 4 + j]);
        }
    }

  gsl_eigen_symmv_workspace *work = gsl_eigen_symmv_alloc (4);
  status = (work != NULL) ? 0 : -1;
  if (status != 0)
    goto gsl_mat_exit;

  gsl_vector *eigenvals = gsl_vector_calloc (4);
  if (eigenvals == NULL)
    goto gsl_exit;

  gsl_matrix *eigenvecs = gsl_matrix_alloc (4, 4);
  if (eigenvecs == NULL)
    goto gsl_exit1;

  gsl_eigen_symmv (mat, eigenvals, eigenvecs, work);

  puts ("Eigenvalues:");
  for (size_t i = 0; i < 4; i++)
    {
      printf ("%2.4f ", gsl_vector_get (eigenvals, i));
    }
  putchar ('\n');

  puts ("---------------");

  puts ("Raw matrix:");
  for (size_t i = 0; i < 4; i++)
    {
      for (size_t j = 0; j < 4; j++)
        {
          printf ("%2.4f ", gsl_matrix_get (mat, i, j));
        }
      putchar ('\n');
    }

  puts ("---------------");

  for (size_t i = 0; i < 4; i++)
    {
      printf ("Eigenvector %ld:\n", i + 1);
      for (size_t j = 0; j < 4; j++)
        {
          printf ("%2.4f ", gsl_matrix_get (mat, j, i));
        }
      putchar ('\n');
    }

  gsl_matrix_free (eigenvecs);
gsl_exit1:
  gsl_vector_free (eigenvals);
gsl_exit:
  gsl_eigen_symmv_free (work);
gsl_mat_exit: // exit on matrix init failure
  gsl_matrix_free (mat);
prg_exit:
  free_obj_file (&obj);
  return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
