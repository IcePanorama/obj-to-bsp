#include "utils.h"
#include "log.h"

struct VertexCoord_s
calc_centroid_from_obj (ObjFile_t o[static 1])
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
                                 float output[16])
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

  x_var /= o->num_faces * 3;
  y_var /= o->num_faces * 3;
  z_var /= o->num_faces * 3;
  w_var /= o->num_faces * 3;

  output[0] = x_var;
  output[5] = y_var;
  output[10] = z_var;
  output[15] = w_var;
}

static void
calc_covars_from_obj_centroid (ObjFile_t *o, struct VertexCoord_s *c,
                               float output[16])
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

  x_y_covar /= o->num_faces * 3;
  x_z_covar /= o->num_faces * 3;
  x_w_covar /= o->num_faces * 3;
  y_z_covar /= o->num_faces * 3;
  y_w_covar /= o->num_faces * 3;
  z_w_covar /= o->num_faces * 3;

  output[1] = output[4] = x_y_covar;
  output[2] = output[8] = x_z_covar;
  output[3] = output[12] = x_w_covar;

  output[6] = output[9] = y_z_covar;
  output[7] = output[13] = y_w_covar;

  output[11] = output[14] = z_w_covar;
}

void
calc_covar_mat_from_obj_centroid (ObjFile_t o[static 1],
                                  struct VertexCoord_s c[static 1],
                                  float output[16])
{
  calculate_var_from_obj_centroid (o, c, output);
  calc_covars_from_obj_centroid (o, c, output);
}

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
