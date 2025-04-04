#include "utils.h"
#include "log.h"

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix_double.h>

void
print_covar_mat (float m[16])
{
  LOG_DEBUG_INFO ("%s\n", "Covariance Matrix:");
  for (size_t i = 0; i < 4; i++)
    {
      char strs[4][12] = { 0 };
      for (size_t j = 0; j < 4; j++)
        {
          sprintf (strs[j], "%3.6f ", m[i * 4 + j]);
        }
      LOG_DEBUG_INFO ("%11s %11s %11s %11s%c", strs[0], strs[1], strs[2],
                      strs[3], '\n');
    }
}

static void
convert_1d_arr_to_mat (float in[16], gsl_matrix *out)
{
  for (size_t i = 0; i < 4; i++)
    {
      for (size_t j = 0; j < 4; j++)
        {
          gsl_matrix_set (out, i, j, in[i * 4 + j]);
        }
    }
}

int
calc_eigenvals_vecs (float in[16], float eigenvals[4], float eigenvecs[4][4])
{
  gsl_eigen_symmv_workspace *work = gsl_eigen_symmv_alloc (4);
  if (work == NULL)
    {
      LOG_ERROR_MSG ("Failed to alloc gsl eigen symmv workspace.\n");
      return -1;
    }

  gsl_matrix *mat = gsl_matrix_alloc (4, 4);
  if (mat == NULL)
    {
      gsl_eigen_symmv_free (work);
      LOG_ERROR_MSG (
          "Failed to alloc gsl matrix of size 4x4 for eigen symmv input.\n");
      return -1;
    }

  convert_1d_arr_to_mat (in, mat);

  gsl_vector *eval = gsl_vector_calloc (4);
  if (eval == NULL)
    {
      gsl_matrix_free (mat);
      gsl_eigen_symmv_free (work);
      LOG_ERROR_MSG ("Failed to alloc gsl vector of size 4.\n");
      return -1;
    }

  gsl_matrix *evec = gsl_matrix_alloc (4, 4);
  if (evec == NULL)
    {
      gsl_vector_free (eval);
      gsl_matrix_free (mat);
      gsl_eigen_symmv_free (work);
      LOG_ERROR_MSG (
          "Failed to alloc gsl matrix of size 4x4 for eigenvectors.\n");
      return -1;
    }

  gsl_eigen_symmv (mat, eval, evec, work);
  gsl_matrix_free (mat);
  gsl_eigen_symmv_free (work);

  for (size_t i = 0; i < 4; i++)
    {
      eigenvals[i] = gsl_vector_get (eval, i);
    }
  gsl_vector_free (eval);

  /* Convert evec from column major order to row major order. */
  for (size_t i = 0; i < 4; i++)
    {
      for (size_t j = 0; j < 4; j++)
        {
          eigenvecs[i][j] = gsl_matrix_get (evec, j, i);
        }
    }
  gsl_matrix_free (evec);

  return 0;
}

void
get_largest_evec_from_largest_eval (float eval[static 4],
                                    float evec[static 4][4],
                                    float **largest_evec)
{
  size_t largest_idx = 0;
  for (size_t i = 1; i < 4; i++)
    {
      if (eval[i] >= eval[largest_idx])
        largest_idx = i;
    }
  (*largest_evec) = evec[largest_idx];
}

float
signed_dist (float from[4], float from_norm[4], float to[4])
{
  float work[4] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      work[i] = from[i] - to[i];
    }

  float output = 0;
  for (size_t i = 0; i < 4; i++)
    {
      output += from_norm[i] * work[i];
    }

  return output;
}
