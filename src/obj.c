#include "obj.h"
#include "dynamic_arr.h"
#include "log.h"

#include <string.h>

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
      o->vertices_list = dyna_create (sizeof (struct VertexCoord_s));
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
      struct VertexCoord_s tmp = { .x = x, .y = y, .z = z, .w = w };
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
              curr.verts[cnt] = (struct VertexCoord_s *)dyna_at (
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
          /** If optional texture coords not provided, skip. */
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
  if (o->vertices_list != NULL)
    dyna_free (o->vertices_list);
  if (o->texture_coords_list != NULL)
    dyna_free (o->texture_coords_list);
  if (o->vertex_normals_list != NULL)
    dyna_free (o->vertex_normals_list);
  if (o->parameter_space_vertices_list != NULL)
    dyna_free (o->parameter_space_vertices_list);
  if (o->faces_list != NULL)
    dyna_free (o->faces_list);
}
