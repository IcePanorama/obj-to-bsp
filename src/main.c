#include "log.h"
#include "obj.h"

#include <stdio.h>
#include <stdlib.h>

/**
 *  If you'd like to verify the output of this calculation in blender, export
 *  your obj file with the following settings:
 *  + Forward Axis: Y
 *  + Up Axis: Z
 */
static struct VertexCoord_s
calculate_centroid_from_obj (ObjFile_t o[static 1])
{
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

static void
calculate_var_from_obj_centroid (ObjFile_t *o, struct VertexCoord_s *c,
                                 float output[4][4])
{
  float x_var = 0.0;
  float y_var = 0.0;
  float z_var = 0.0;
  float w_var = 0.0;

  for (size_t i = 0; i < o->num_faces; i++)
    {
      struct PolygonalFace_s *curr = &o->faces_list[i];
      for (size_t j = 0; j < 3; j++)
        {
          x_var += (o->verticies_list[curr->vertices[j]].x - c->x)
                   * (o->verticies_list[curr->vertices[j]].x - c->x);
          y_var += (o->verticies_list[curr->vertices[j]].y - c->y)
                   * (o->verticies_list[curr->vertices[j]].y - c->y);
          z_var += (o->verticies_list[curr->vertices[j]].z - c->z)
                   * (o->verticies_list[curr->vertices[j]].z - c->z);
          w_var += (o->verticies_list[curr->vertices[j]].w - c->w)
                   * (o->verticies_list[curr->vertices[j]].w - c->w);
        }
    }

  x_var /= o->num_verticies;
  y_var /= o->num_verticies;
  z_var /= o->num_verticies;
  w_var /= o->num_verticies;

  output[0][0] = x_var;
  output[1][1] = y_var;
  output[2][2] = z_var;
  output[3][3] = w_var;
}

static void
calc_covars_from_obj_centroid (ObjFile_t *o, struct VertexCoord_s *c,
                               float output[4][4])
{
  float x_y_covar = 0.0;
  float x_z_covar = 0.0;
  float x_w_covar = 0.0;
  float y_z_covar = 0.0;
  float y_w_covar = 0.0;
  float z_w_covar = 0.0;

  for (size_t i = 0; i < o->num_faces; i++)
    {
      struct PolygonalFace_s *curr = &o->faces_list[i];
      for (size_t j = 0; j < 3; j++)
        {
          x_y_covar += (o->verticies_list[curr->vertices[j]].x - c->x)
                       * ((o->verticies_list[curr->vertices[j]].y - c->y));
          x_z_covar += (o->verticies_list[curr->vertices[j]].x - c->x)
                       * ((o->verticies_list[curr->vertices[j]].z - c->z));
          x_w_covar += (o->verticies_list[curr->vertices[j]].x - c->x)
                       * ((o->verticies_list[curr->vertices[j]].w - c->w));
          y_z_covar += (o->verticies_list[curr->vertices[j]].y - c->y)
                       * ((o->verticies_list[curr->vertices[j]].z - c->z));
          y_w_covar += (o->verticies_list[curr->vertices[j]].y - c->y)
                       * ((o->verticies_list[curr->vertices[j]].w - c->w));
          z_w_covar += (o->verticies_list[curr->vertices[j]].z - c->z)
                       * ((o->verticies_list[curr->vertices[j]].w - c->w));
        }
    }

  x_y_covar /= o->num_verticies;
  x_z_covar /= o->num_verticies;
  x_w_covar /= o->num_verticies;
  y_z_covar /= o->num_verticies;
  y_w_covar /= o->num_verticies;
  z_w_covar /= o->num_verticies;

  output[0][1] = output[1][0] = x_y_covar;
  output[0][2] = output[2][0] = x_z_covar;
  output[0][3] = output[3][0] = x_w_covar;

  output[1][2] = output[2][1] = y_z_covar;
  output[1][3] = output[3][1] = y_w_covar;

  output[2][3] = output[3][2] = z_w_covar;
}

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
static void
calculate_covariance_mat_from_obj_centroid (ObjFile_t *o,
                                            struct VertexCoord_s *c,
                                            float output[4][4])
{
  calculate_var_from_obj_centroid (o, c, output);
  calc_covars_from_obj_centroid (o, c, output);
}

static void
print_covar_mat (float m[4][4])
{
  puts ("Covariance matrix:");
  for (size_t i = 0; i < 4; i++)
    {
      for (size_t j = 0; j < 4; j++)
        {
          printf ("%3.6f ", m[i][j]);
        }
      putchar ('\n');
    }
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

  float covar_mat[4][4] = { 0 };
  calculate_covariance_mat_from_obj_centroid (&obj, &centroid, covar_mat);
  printf ("Variances: %f, %f, %f, %f\n", covar_mat[0][0], covar_mat[1][1],
          covar_mat[2][2], covar_mat[3][3]);

  print_covar_mat (covar_mat);

  free_obj_file (&obj);
  return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
