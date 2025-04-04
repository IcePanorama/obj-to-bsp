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

  return u_calc_dot_product (from_norm, work);
}

float
u_calc_dot_product (float a[static 4], float b[static 4])
{
  float res = 0.0;
  for (size_t i = 0; i < 4; i++)
    {
      res += a[i] * b[i];
    }

  return res;
}

float
u_calc_line_plane_intersection (float v1[static 4], float v2[static 4],
                                float plane_cen[static 4],
                                float plane_norm[static 4])
{
  float neg_norm[]
      = { -plane_norm[0], -plane_norm[1], -plane_norm[2], -plane_norm[3] };
  float dist2cen[] = { v1[0] - plane_cen[0], v1[1] - plane_cen[1],
                       v1[2] - plane_cen[2], v1[3] - plane_cen[3] };
  float distBetPoints[]
      = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2], v2[3] - v1[3] };

  float num = u_calc_dot_product (neg_norm, dist2cen);
  float denom = u_calc_dot_product (plane_norm, distBetPoints);
  return num / denom;
}
