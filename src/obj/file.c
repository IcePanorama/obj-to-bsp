#include "obj/file.h"
#include "dyna.h"
#include "obj/errors.h"
#include "obj/obj.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
#include "dynamic_arr.h"
#include "log.h"
#include "obj/face.h"
#include "obj/param_space_vertex.h"
#include "obj/tex_coord.h"
#include "obj/vertex_coord.h"
#include "obj/vertex_norm.h"

#include <string.h>
*/

struct ObjFile_s
{
  char *path;
  DynamicArr_t *objs;
};

/*
static int
process_new_vertex_norm (ObjFile_t *o, const char *input)
{
  if (o->vertex_normals_list == NULL)
    {
      o->vertex_normals_list = dyna_create (sizeof (struct VertexNormal_s));
      if (o->vertex_normals_list == NULL)
        {
          LOG_ERROR_MSG ("Error creating vertex normals list.\n");
          return -1;
        }
    }

  float x = 0.0;
  float y = 0.0;
  float z = 0.0;
  if (sscanf (input, "vn %f %f %f", &x, &y, &z) == 3)
    {
      struct VertexNormal_s tmp = { .x = x, .y = y, .z = z };
      if (dyna_append (o->vertex_normals_list, (void *)&tmp) != 0)
        return -1;
      LOG_DEBUG_INFO ("vn %f %f %f\n", x, y, z);
    }
  else
    {
      LOG_ERROR ("Error parsing line: '%s'.\n", input);
      return -1;
    }

  return 0;
}

static int
process_new_parameter_space_vertex (ObjFile_t *o, const char *input)
{
  if (o->parameter_space_vertices_list == NULL)
    {
      o->parameter_space_vertices_list
          = dyna_create (sizeof (struct ParameterSpaceVertex_s));
      if (o->parameter_space_vertices_list == NULL)
        {
          LOG_ERROR_MSG ("Error creating parameter space vertices list.\n");
          return -1;
        }
    }

  float u = 0.0;
  float v = 0.0;
  float w = 0.0;
  if (sscanf (input, "vp %f %f %f", &u, &v, &w) >= 1)
    {
      struct ParameterSpaceVertex_s tmp = { .u = u, .v = v, .w = w };
      if (dyna_append (o->parameter_space_vertices_list, (void *)&tmp) != 0)
        return -1;

      LOG_DEBUG_INFO ("vp %f %f %f\n", u, v, w);
    }
  else
    {
      LOG_ERROR ("Error parsing input: '%s'.\n", input);
      return -1;
    }

  return 0;
}

static int
process_new_texture_coords (ObjFile_t *o, const char *input)
{
  if (o->texture_coords_list == NULL)
    {
      o->texture_coords_list = dyna_create (sizeof (struct TextureCoord_s));
      if (o->texture_coords_list == NULL)
        {
          LOG_ERROR_MSG ("Error creating texture coordinates list.\n");
          return -1;
        }
    }

  float u = 0.0;
  float v = 0.0;
  float w = 0.0;
  if (sscanf (input, "vt %f %f %f", &u, &v, &w) >= 1)
    {
      struct TextureCoord_s tmp = { .u = u, .v = v, .w = w };
      LOG_DEBUG_INFO ("vt %f %f %f\n", u, v, w);
      if (dyna_append (o->texture_coords_list, (void *)&tmp) != 0)
        return -1;
    }
  else
    {
      LOG_ERROR ("Error parsing input: '%s'.\n", input);
      return -1;
    }

  return 0;
}

static int
process_new_vertex_coordinates (ObjFile_t *o, const char *line)
{
  if (o->vertices_list == NULL)
    {
      o->vertices_list = dyna_create (sizeof (struct OBJVertexCoord_s));
      if (o->vertices_list == NULL)
        {
          LOG_ERROR_MSG ("Error creating vertices list.\n");
          return -1;
        }
    }

  float x = 0.0;
  float y = 0.0;
  float z = 0.0;
  float w = 1.0;
  if (sscanf (line, "v %f %f %f %f", &x, &y, &z, &w) >= 3)
    {
      struct OBJVertexCoord_s tmp = { .x = x, .y = y, .z = z, .w = w };
      if (dyna_append (o->vertices_list, (void *)&tmp) != 0)
        return -1;

      LOG_DEBUG_INFO ("v %f %f %f %f\n", x, y, z, w);
    }
  else
    {
      LOG_ERROR ("Error parsing line: '%s'.\n", line);
      return -1;
    }

  return 0;
}

static int
process_new_face (ObjFile_t *o, const char *input)
{
  if (o->faces_list == NULL)
    {
      o->faces_list = dyna_create (sizeof (struct PolygonalFace_s));
      if (o->faces_list == NULL)
        {
          LOG_ERROR_MSG ("Error creating faces list.\n");
          return -1;
        }
    }

  char input_cpy[256];
  strncpy (input_cpy, input, 255);
  input_cpy[strlen (input_cpy) - 1] = ' '; // remove '\n'

  size_t cnt = 0;
  char *input_ptr = input_cpy + 2; // skip "f "
  char *token = strchr (input_ptr, ' ');
  struct PolygonalFace_s curr = { 0 };

  while (token != NULL)
    {
      *token = '\0';
      token++;

      if (cnt >= 3)
        {
          LOG_ERROR ("%ld-dimensional polygon faces unsupported. Please use "
                     "triangles.\n",
                     cnt);
          return -1;
        }

      size_t num_parts = 0;
      char *idx = strtok (input_ptr, "/");
      while (idx != NULL)
        {
          if (num_parts == 0)
            {
              if ((size_t)(atoi (idx) - 1) >= dyna_get_size (o->vertices_list))
                {
                  LOG_ERROR ("Invalid vertex index, %d.\n", atoi (idx) - 1);
                  return -1;
                }
              curr.verts[cnt] = (struct OBJVertexCoord_s *)dyna_at (
                  o->vertices_list, atoi (idx) - 1);
            }
          else if (num_parts == 1)
            {
              if ((size_t)(atoi (idx) - 1)
                  >= dyna_get_size (o->texture_coords_list))
                {
                  LOG_ERROR ("Invalid texture coordinate index, %d.\n",
                             atoi (idx) - 1);
                  return -1;
                }
              curr.tex_coords[cnt]
                  = dyna_at (o->texture_coords_list, atoi (idx) - 1);
            }
          else if (num_parts == 2)
            {
              if ((size_t)(atoi (idx) - 1)
                  >= dyna_get_size (o->vertex_normals_list))
                {
                  LOG_ERROR ("Invalid vertex normal index, %d.\n",
                             atoi (idx) - 1);
                  return -1;
                }
              curr.norms[cnt]
                  = dyna_at (o->vertex_normals_list, atoi (idx) - 1);
            }

          num_parts++;
          ** If optional texture coords not provided, skip. *
          if ((idx + strlen (idx) + 1)[0] == '/')
            num_parts++;
          idx = strtok (NULL, "/");
        }

      input_ptr = token;
      cnt++;
      token = strchr (input_ptr, ' ');
    }

  if (dyna_append (o->faces_list, (void *)&curr) != 0)
    return -1;

  LOG_DEBUG_INFO ("%s", input);

  return 0;
}

static int
process_verts_txt_coords (ObjFile_t o[static 1], FILE *fptr)
{
  if (fseek (fptr, 0x0, SEEK_SET) != 0)
    {
      LOG_ERROR_MSG ("Failed to seek to beginning of file.\n");
      return -1;
    }

  char line[256];
  while (fgets (line, sizeof (line), fptr) != NULL)
    {
      if (line[0] == '#') // ignore comments
        continue;
      else if (strncmp (line, "vn", 2) == 0)
        {
          if (process_new_vertex_norm (o, line) != 0)
            return -1;
        }
      else if (strncmp (line, "vp", 2) == 0)
        {
          if (process_new_parameter_space_vertex (o, line) != 0)
            return -1;
        }
      else if (strncmp (line, "vt", 2) == 0)
        {
          if (process_new_texture_coords (o, line) != 0)
            return -1;
        }
      else if (line[0] == 'v')
        {
          if (process_new_vertex_coordinates (o, line) != 0)
            return -1;
        }
    }

  return 0;
}

static int
process_faces (ObjFile_t o[static 1], FILE fptr[static 1])
{
  if (fseek (fptr, 0x0, SEEK_SET) != 0)
    {
      LOG_ERROR_MSG ("Failed to seek to beginning of file.\n");
      return -1;
    }

  char line[256];
  while (fgets (line, sizeof (line), fptr) != NULL)
    {
      if (line[0] == 'f')
        {
          if (process_new_face (o, line) != 0)
            return -1;
        }
    }

  return 0;
}
*/

/*
int
obj_calc_centroid (ObjFile_t *o, float centroid[static 4])
{
  if (o == NULL)
    return -1;

  float x_total = 0.0;
  float y_total = 0.0;
  float z_total = 0.0;
  float w_total = 0.0;

  size_t num_faces = dyna_get_size (o->faces_list);
  for (size_t i = 0; i < num_faces; i++)
    {
      struct PolygonalFace_s *curr
          = (struct PolygonalFace_s *)dyna_at (o->faces_list, i);
      x_total += curr->verts[0]->x;
      x_total += curr->verts[1]->x;
      x_total += curr->verts[2]->x;

      y_total += curr->verts[0]->y;
      y_total += curr->verts[1]->y;
      y_total += curr->verts[2]->y;

      z_total += curr->verts[0]->z;
      z_total += curr->verts[1]->z;
      z_total += curr->verts[2]->z;

      w_total += curr->verts[0]->w;
      w_total += curr->verts[1]->w;
      w_total += curr->verts[2]->w;
    }

  x_total /= num_faces * 3;
  y_total /= num_faces * 3;
  z_total /= num_faces * 3;
  w_total /= num_faces * 3;

  centroid[0] = x_total;
  centroid[1] = y_total;
  centroid[2] = z_total;
  centroid[3] = w_total;
  return 0;
}

static void
calculate_var_from_obj_centroid (ObjFile_t *o, float c[static 4],
                                 float output[16])
{
  float x_var = 0.0;
  float y_var = 0.0;
  float z_var = 0.0;
  float w_var = 0.0;

  const float cx = c[0];
  const float cy = c[1];
  const float cz = c[2];
  const float cw = c[3];

  size_t num_faces = dyna_get_size (o->faces_list);
  for (size_t i = 0; i < num_faces; i++)
    {
      struct PolygonalFace_s *curr
          = (struct PolygonalFace_s *)dyna_at (o->faces_list, i);
      for (size_t j = 0; j < 3; j++)
        {
          x_var += (curr->verts[j]->x - cx) * (curr->verts[j]->x - cx);
          y_var += (curr->verts[j]->y - cy) * (curr->verts[j]->y - cy);
          z_var += (curr->verts[j]->z - cz) * (curr->verts[j]->z - cz);
          w_var += (curr->verts[j]->w - cw) * (curr->verts[j]->w - cw);
        }
    }

  x_var /= num_faces * 3;
  y_var /= num_faces * 3;
  z_var /= num_faces * 3;
  w_var /= num_faces * 3;

  output[0] = x_var;
  output[5] = y_var;
  output[10] = z_var;
  output[15] = w_var;
}

DynamicArray_t *
obj_get_faces_list (ObjFile_t *o)
{
  if (o == NULL)
    return NULL;

  return o->faces_list;
}

static void
calc_covars_from_obj_centroid (ObjFile_t *o, float c[static 4],
                               float output[16])
{
  float x_y_covar = 0.0;
  float x_z_covar = 0.0;
  float x_w_covar = 0.0;
  float y_z_covar = 0.0;
  float y_w_covar = 0.0;
  float z_w_covar = 0.0;

  const float cx = c[0];
  const float cy = c[1];
  const float cz = c[2];
  const float cw = c[3];

  DynamicArray_t *faces_list = obj_get_faces_list (o);
  size_t num_faces = dyna_get_size (faces_list);
  for (size_t i = 0; i < num_faces; i++)
    {
      struct PolygonalFace_s *curr
          = (struct PolygonalFace_s *)dyna_at (faces_list, i);
      for (size_t j = 0; j < 3; j++)
        {
          x_y_covar += (curr->verts[j]->x - cx) * (curr->verts[j]->y - cy);
          x_z_covar += (curr->verts[j]->x - cx) * (curr->verts[j]->z - cz);
          x_w_covar += (curr->verts[j]->x - cx) * (curr->verts[j]->w - cw);
          y_z_covar += (curr->verts[j]->y - cy) * (curr->verts[j]->z - cz);
          y_w_covar += (curr->verts[j]->y - cy) * (curr->verts[j]->w - cw);
          z_w_covar += (curr->verts[j]->z - cz) * (curr->verts[j]->w - cw);
        }
    }

  x_y_covar /= num_faces * 3;
  x_z_covar /= num_faces * 3;
  x_w_covar /= num_faces * 3;
  y_z_covar /= num_faces * 3;
  y_w_covar /= num_faces * 3;
  z_w_covar /= num_faces * 3;

  output[1] = output[4] = x_y_covar;
  output[2] = output[8] = x_z_covar;
  output[3] = output[12] = x_w_covar;

  output[6] = output[9] = y_z_covar;
  output[7] = output[13] = y_w_covar;

  output[11] = output[14] = z_w_covar;
  return;
}

int
obj_calc_covar_mat_w_centroid (ObjFile_t *o, float c[static 4],
                               float output[16])
{
  if (o == NULL)
    return -1;

  calculate_var_from_obj_centroid (o, c, output);
  calc_covars_from_obj_centroid (o, c, output);

  return 0;
}
*/

int
process_file (ObjFile_t o[static 1])
{
  FILE *fptr = fopen (o->path, "r");
  if (!fptr)
    {
      fprintf (stderr, "%s: Error opening file: %s\n", __func__, o->path);
      return -1;
    }

  // LO: Reimplement file parsing.
  char *l = NULL;
  size_t l_size = 0;
  while ((getline (&l, &l_size, fptr)) != -1)
    {
      if (l_size == 0)
        {
          if (l)
            free (l);
          l = NULL;
          break;
        }
      else if (l[0] == '#')
        continue;
      else if (strncmp (l, "o ", 2) == 0)
        {
          _OBJObj_t *curr_o = objo_alloc (l + 2, fptr);
          if (!curr_o)
            goto loop_err_exit;

          objo_free (curr_o);
          curr_o = NULL;
        }
      else
        {
          fprintf (stderr, "Malformed input: %s", l);
          goto loop_err_exit;
        }

      free (l);
      l = NULL;
      break;
    }

  fclose (fptr);
  return 0;
loop_err_exit:
  free (l);
  l = NULL;
  fclose (fptr);
  return -1;
}

ObjFile_t *
obj_alloc (char path[static 1])
{
  ObjFile_t *o = calloc (1, sizeof (ObjFile_t));
  if (!o)
    {
      WAVOBJ_OOM_ERR ();
      return NULL;
    }

  o->objs = DynA_alloc (objo_size ());
  if (!o->objs)
    {
      WAVOBJ_OOM_ERR ();
      obj_free (o);
      o = NULL;
      return NULL;
    }

  o->path = strdup (path);
  if (!o->path)
    {
      WAVOBJ_OOM_ERR ();
      obj_free (o);
      o = NULL;
      return NULL;
    }

  if (process_file (o) != 0)
    {
      obj_free (o);
      o = NULL;
      return NULL;
    }

  return o;
}

void
obj_free (ObjFile_t *o)
{
  if (o == NULL)
    return;

  if (o->path)
    free (o->path);
  o->path = NULL;

  if (o->objs)
    DynA_free (o->objs);
  o->objs = NULL;

  free (o);
  o = NULL;
}
