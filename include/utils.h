#ifndef _COVARIANCE_MATRIX_H_
#define _COVARIANCE_MATRIX_H_

#include "obj.h"

/**
 *  Calculates the centroid of a given OBJ file. If you'd like to verify the
 *  output of this calculation in blender, export your obj file with the
 *  following settings:
 *  + Forward Axis: Y
 *  + Up Axis: Z
 *  FIXME: should this be in the obj file?
 */
void calc_centroid_from_obj (float centroid[static 4], ObjFile_t o[static 1]);

/**
 *  Calculates covariance matrix from OBJ file and centroid.
 *
 *  Covariance matrix structure:
 *  __                                               __
 *  |      var(x) covar(x, y) covar(x, z) covar(x, w) |
 *  | covar(x, y)      var(y) covar(y, z) covar(y, w) |
 *  | covar(x, z) covar(y, z)      var(z) covar(z, w) |
 *  | covar(x, w) covar(y, w) covar(z, w)      var(w) |
 *  --                                               --
 *
 *  Note that the matrix is mirrored across the diagonal. Going to be using
 *  population variance/covariance for these calculations.
 *
 *  population_variance(x) = sum(from: 0, to: n - 1, (x_i - mean)^2) / n
 *  population_covariance(x, y) =
 *    sum(from: 0, to: n - 1, (x_i - mean_x)(y_i - mean_y)) / n
 *
 *  where n = `o->num_faces * 3` (as we're dealing with triangles).
 *
 *  See: https://www.geeksforgeeks.org/covariance-matrix/
 *  FIXME: should this be in the obj file?
 */
void calc_covar_mat_from_obj_centroid (ObjFile_t o[static 1],
                                       float c[static 4], float output[16]);

/** Prints the given covariance matrix, `m`, via `LOG_DEBUG_INFO`.*/
void print_covar_mat (float m[16]);

/**
 *  Calculates eigenvalues and eigenvectors for `in` using gsl for all the hard
 *  work. `eigenvecs` is converted from column- to row-major order internally.
 *  No particular reason why, that just seems to make more sense for
 *  me ¯\_(ツ)_/¯.
 *  Param:  in         matrix from which the calculations should be made
 *  Param:  eigenvals  output of eigenvalue calculation
 *  Param:  eigenvecs  output of eigenvector calculation
 *  Returns: zero on success, non-zero on failure.
 */
int calc_eigenvals_vecs (float in[static 16], float eigenvals[static 4],
                         float eigenvecs[static 4][4]);

void get_largest_evec_from_largest_eval (float eval[static 4],
                                         float evec[static 4][4],
                                         float **largest_evec);

float signed_dist (float from[4], float from_norm[4], float to[4]);

#endif /* _COVARIANCE_MATRIX_H_ */
