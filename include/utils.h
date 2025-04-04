#ifndef _COVARIANCE_MATRIX_H_
#define _COVARIANCE_MATRIX_H_

// #include "obj/file.h"

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
