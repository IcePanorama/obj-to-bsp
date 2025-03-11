#include "log.h"
#include "obj.h"

#include <stdlib.h>

/**
 *  If you'd like to verify the output of this calculation in blender, export
 *  your obj file with the following settings:
 *  + Forward Axis: Y
 *  + Up Axis: Z
 */
static struct VertexCoord_s
calculate_centroid_from_obj (ObjFile_t *o)
{
  /**
   *  If you'd like to verify the output of this calculation in blender,
   *  export your obj file with the following settings:
   *  + Forward Axis: Y
   *  + Up Axis: Z
   */
  float x_total = 0.0;
  float y_total = 0.0;
  float z_total = 0.0;
  float w_total = 0.0;
  for (size_t i = 0; i < o->num_faces; i++)
    {
      struct PolygonalFace_s *curr = &o->faces_list[i];
      x_total += o->verticies_list[curr->vertices[0]].x;
      x_total += o->verticies_list[curr->vertices[1]].x;
      x_total += o->verticies_list[curr->vertices[2]].x;

      y_total += o->verticies_list[curr->vertices[0]].y;
      y_total += o->verticies_list[curr->vertices[1]].y;
      y_total += o->verticies_list[curr->vertices[2]].y;

      z_total += o->verticies_list[curr->vertices[0]].z;
      z_total += o->verticies_list[curr->vertices[1]].z;
      z_total += o->verticies_list[curr->vertices[2]].z;

      w_total += o->verticies_list[curr->vertices[0]].w;
      w_total += o->verticies_list[curr->vertices[1]].w;
      w_total += o->verticies_list[curr->vertices[2]].w;
    }

  x_total /= o->num_faces * 3;
  y_total /= o->num_faces * 3;
  z_total /= o->num_faces * 3;
  w_total /= o->num_faces * 3;

  return (struct VertexCoord_s){ x_total, y_total, z_total, w_total };
}

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

  struct VertexCoord_s centroid = calculate_centroid_from_obj (&obj);
  printf ("Centroid: (%f, %f, %f, %f)\n", centroid.x, centroid.y, centroid.z,
          centroid.w);

  /**
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
   *  where n = `obj.num_faces`.
   *
   *  See: https://www.geeksforgeeks.org/covariance-matrix/
   */

  free_obj_file (&obj);
  return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
