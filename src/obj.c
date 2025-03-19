#include "obj.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
create_list (void **list, size_t size)
{
  *list = calloc (1, size);
  if (*list == NULL)
    return -1;

  return 0;
}

static int
resize_list (void **list, size_t new_size)
{
  void *tmp = realloc (*list, new_size);
  if (tmp == NULL)
    return -1;

  *list = tmp;
  return 0;
}

static int
process_new_vertex_norm (ObjFile_t *o, const char *input)
{
  if (o->max_num_vertex_normals == 0)
    {
      if (create_list ((void **)&o->vertex_normals_list,
                       sizeof (struct VertexNormal_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating vertex normals list.\n");
          return -1;
        }

      o->max_num_vertex_normals = 1;
    }

  if (o->num_vertex_normals == o->max_num_vertex_normals)
    {
      o->max_num_vertex_normals *= 2;
      if (resize_list ((void **)&o->vertex_normals_list,
                       sizeof (struct VertexNormal_s)
                           * o->max_num_vertex_normals)
          != 0)
        {
          LOG_ERROR ("Error resizing vertex normals list to size %ld.\n",
                     o->max_num_vertex_normals);
          return -1;
        }
    }

  float x, y, z;
  if (sscanf (input, "vn %f %f %f", &x, &y, &z) == 3)
    {
      o->vertex_normals_list[o->num_vertex_normals].x = x;
      o->vertex_normals_list[o->num_vertex_normals].y = y;
      o->vertex_normals_list[o->num_vertex_normals].z = z;

      LOG_DEBUG_INFO ("vn %f %f %f\n", x, y, z);

      o->num_vertex_normals++;
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
  if (o->max_num_parameter_space_verticies == 0)
    {
      if (create_list ((void **)&o->parameter_space_verticies_list,
                       sizeof (struct ParameterSpaceVertex_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating parameter space verticies list.\n");
          return -1;
        }

      o->max_num_parameter_space_verticies = 1;
    }

  if (o->num_parameter_space_verticies == o->max_num_parameter_space_verticies)
    {
      o->max_num_parameter_space_verticies *= 2;
      if (resize_list ((void **)&o->parameter_space_verticies_list,
                       sizeof (struct ParameterSpaceVertex_s)
                           * o->max_num_parameter_space_verticies)
          != 0)
        {
          LOG_ERROR (
              "Error resizing parameter space verticies list to size %ld.\n",
              o->max_num_parameter_space_verticies);
          return -1;
        }
    }

  float u, v = 0.0, w = 0.0;
  if (sscanf (input, "vp %f %f %f", &u, &v, &w) >= 1)
    {
      /* clang-format off */
      o->parameter_space_verticies_list[o ->num_parameter_space_verticies].u = u;
      o->parameter_space_verticies_list[o ->num_parameter_space_verticies].v = v;
      o->parameter_space_verticies_list[o ->num_parameter_space_verticies].w = w;
      /* clang-format on */

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
  if (o->max_num_texture_coords == 0)
    {
      if (create_list ((void **)&o->texture_coords_list,
                       sizeof (struct TextureCoord_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating texture coordinates list.\n");
          return -1;
        }

      o->max_num_texture_coords = 1;
    }

  if (o->num_texture_coords == o->max_num_texture_coords)
    {
      o->max_num_texture_coords *= 2;
      if (resize_list ((void **)&o->texture_coords_list,
                       sizeof (struct TextureCoord_s)
                           * o->max_num_texture_coords)
          != 0)
        {
          LOG_ERROR ("Error resizing texture coordinates list to size %ld.\n",
                     o->max_num_texture_coords);
          return -1;
        }
    }

  float u, v = 0.0, w = 0.0;
  if (sscanf (input, "vt %f %f %f", &u, &v, &w) >= 1)
    {
      o->texture_coords_list[o->num_texture_coords].u = u;
      o->texture_coords_list[o->num_texture_coords].v = v;
      o->texture_coords_list[o->num_texture_coords].w = w;

      LOG_DEBUG_INFO ("vt %f %f %f\n", u, v, w);
    }
  else
    {
      LOG_ERROR ("Error parsing input: '%s'.\n", input);
      return -1;
    }

  o->num_texture_coords++;
  return 0;
}

static int
process_new_vertex_coordinates (ObjFile_t *o, const char *line)
{
  if (o->max_num_verticies == 0)
    {
      if (create_list ((void **)&o->verticies_list,
                       sizeof (struct VertexCoord_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating verticies list.\n");
          return -1;
        }

      o->max_num_verticies = 1;
    }

  if (o->num_verticies == o->max_num_verticies)
    {
      o->max_num_verticies *= 2;
      if (resize_list ((void **)&o->verticies_list,
                       sizeof (struct VertexCoord_s) * o->max_num_verticies)
          != 0)
        {
          LOG_ERROR ("Error resizing vertices list to size %ld.\n",
                     o->max_num_verticies);
          return -1;
        }
    }

  float x, y, z, w = 1.0;
  if (sscanf (line, "v %f %f %f %f", &x, &y, &z, &w) >= 3)
    {
      o->verticies_list[o->num_verticies].x = x;
      o->verticies_list[o->num_verticies].y = y;
      o->verticies_list[o->num_verticies].z = z;
      o->verticies_list[o->num_verticies].w = w;

      LOG_DEBUG_INFO ("v %f %f %f %f\n", x, y, z, w);

      o->num_verticies++;
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
  if (o->max_num_faces == 0)
    {
      if (create_list ((void **)&o->faces_list,
                       sizeof (struct PolygonalFace_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating faces list.\n");
          return -1;
        }

      o->max_num_faces = 1;
    }

  if (o->num_faces == o->max_num_faces)
    {
      o->max_num_faces *= 2;
      if (resize_list ((void **)&o->faces_list,
                       sizeof (struct PolygonalFace_s) * o->max_num_faces)
          != 0)
        {
          LOG_ERROR ("Error resizing faces list to size %ld.\n",
                     o->max_num_faces);
          return -1;
        }
    }

  char input_cpy[256];
  strncpy (input_cpy, input, 255);
  input_cpy[strlen (input_cpy) - 1] = ' '; // remove '\n'

  size_t cnt = 0;
  char *input_ptr = input_cpy + 2; // skip "f "
  char *token = strchr (input_ptr, ' ');
  struct PolygonalFace_s *curr = &o->faces_list[o->num_faces];
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
              if ((size_t)(atoi (idx) - 1) >= o->num_verticies)
                {
                  LOG_ERROR ("Invalid vertex index, %d.\n", atoi (idx) - 1);
                  return -1;
                }
              curr->verts[cnt] = &o->verticies_list[atoi (idx) - 1];
            }
          else if (num_parts == 1)
            {
              if ((size_t)(atoi (idx) - 1) >= o->num_texture_coords)
                {
                  LOG_ERROR ("Invalid texture coordinate index, %d.\n",
                             atoi (idx) - 1);
                  return -1;
                }
              curr->tex_coords[cnt] = &o->texture_coords_list[atoi (idx) - 1];
            }
          else if (num_parts == 2)
            {
              if ((size_t)(atoi (idx) - 1) >= o->num_vertex_normals)
                {
                  LOG_ERROR ("Invalid vertex normal index, %d.\n",
                             atoi (idx) - 1);
                  return -1;
                }
              curr->norms[cnt] = &o->vertex_normals_list[atoi (idx) - 1];
            }

          num_parts++;
          /** If optional texture coords not provided, skip. */
          if ((idx + strlen (idx) + 1)[0] == '/')
            num_parts++;
          idx = strtok (NULL, "/");
        }

      input_ptr = token;
      cnt++;
      token = strchr (input_ptr, ' ');
    }

  LOG_DEBUG_INFO ("%s", input);

  o->num_faces++;
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

int
create_obj_file_from_file (ObjFile_t o[static 1],
                           const char file_path[static 1])
{
  FILE *fptr = fopen (file_path, "r");
  if (fptr == NULL)
    {
      LOG_ERROR ("Unable to open file, %s.\n", file_path);
      return -1;
    }

  memset (o, 0, sizeof (ObjFile_t));

  if ((process_verts_txt_coords (o, fptr) != 0)
      || (process_faces (o, fptr) != 0))
    {
      fclose (fptr);
      return -1;
    }

  fclose (fptr);
  return 0;
}

void
free_obj_file (ObjFile_t *o)
{
  if (o->verticies_list != NULL)
    free (o->verticies_list);
  if (o->vertex_normals_list != NULL)
    free (o->vertex_normals_list);
  if (o->parameter_space_verticies_list != NULL)
    free (o->parameter_space_verticies_list);
  if (o->texture_coords_list != NULL)
    free (o->texture_coords_list);
  if (o->faces_list != NULL)
    {
      free (o->faces_list);
    }
}
