#ifndef _WAVEFRONT_OBJ_FILE_H_
#define _WAVEFRONT_OBJ_FILE_H_

#include "dynamic_arr.h"

typedef struct ObjFile_s ObjFile_t;

ObjFile_t *obj_create (const char file_path[static 1]);
void obj_free (ObjFile_t *o);

/**
 *  Calculates the centroid of a given OBJ file. If you'd like to verify the
 *  output of this calculation in blender, export your obj file with the
 *  following settings:
 *  + Forward Axis: Y
 *  + Up Axis: Z
 *  Returns: Zero on success, non-zero on failure.
 */
int obj_calc_centroid (ObjFile_t *o, float centroid[static 4]);

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
 *  where n = num faces * 3 (as we're dealing with triangles).
 *
 *  See: https://www.geeksforgeeks.org/covariance-matrix/
 *  See: `obj_calc_centroid`.
 *  Returns: zero on success, non-zero on failure.
 */
int obj_calc_covar_mat_w_centroid (ObjFile_t *o, float c[static 4],
                                   float output[16]);

DynamicArray_t *obj_get_faces_list (ObjFile_t *o);

#endif /* _WAVEFRONT_OBJ_FILE_H_ */
