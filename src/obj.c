#include "obj.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_BUILD
#include <assert.h>
#endif /* DEBUG_BUILD */

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
process_new_vertex_normal (ObjFile_t *obj_file, const char *input)
{
  if (obj_file->max_num_vertex_normals == 0)
    {
      if (create_list ((void **)&obj_file->vertex_normals_list,
                       sizeof (struct VertexNormal_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating vertex normals list.\n");
          return -1;
        }

      obj_file->max_num_vertex_normals = 1;
    }

  if (obj_file->num_vertex_normals == obj_file->max_num_vertex_normals)
    {
      obj_file->max_num_vertex_normals *= 2;
      if (resize_list ((void **)&obj_file->vertex_normals_list,
                       sizeof (struct VertexNormal_s)
                           * obj_file->max_num_vertex_normals)
          != 0)
        {
          LOG_ERROR ("Error resizing vertex normals list to size %ld.\n",
                     obj_file->max_num_vertex_normals);
          return -1;
        }
    }

  float x, y, z;
  if (sscanf (input, "vn %f %f %f", &x, &y, &z) == 3)
    {
      obj_file->vertex_normals_list[obj_file->num_vertex_normals].x = x;
      obj_file->vertex_normals_list[obj_file->num_vertex_normals].y = y;
      obj_file->vertex_normals_list[obj_file->num_vertex_normals].z = z;

      LOG_DEBUG_INFO ("vn %f %f %f\n", x, y, z);

      obj_file->num_vertex_normals++;
    }
  else
    {
      LOG_ERROR ("Error parsing line: '%s'.\n", input);
      return -1;
    }

  return 0;
}

static int
process_new_parameter_space_vertex (ObjFile_t *obj_file, const char *input)
{
  if (obj_file->max_num_parameter_space_verticies == 0)
    {
      if (create_list ((void **)&obj_file->parameter_space_verticies_list,
                       sizeof (struct ParameterSpaceVertex_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating parameter space verticies list.\n");
          return -1;
        }

      obj_file->max_num_parameter_space_verticies = 1;
    }

  if (obj_file->num_parameter_space_verticies
      == obj_file->max_num_parameter_space_verticies)
    {
      obj_file->max_num_parameter_space_verticies *= 2;
      if (resize_list ((void **)&obj_file->parameter_space_verticies_list,
                       sizeof (struct ParameterSpaceVertex_s)
                           * obj_file->max_num_parameter_space_verticies)
          != 0)
        {
          LOG_ERROR (
              "Error resizing parameter space verticies list to size %ld.\n",
              obj_file->max_num_parameter_space_verticies);
          return -1;
        }
    }

  float u, v = 0.0, w = 0.0;
  if (sscanf (input, "vp %f %f %f", &u, &v, &w) >= 1)
    {
      /* clang-format off */
      obj_file->parameter_space_verticies_list[obj_file ->num_parameter_space_verticies].u = u;
      obj_file->parameter_space_verticies_list[obj_file ->num_parameter_space_verticies].v = v;
      obj_file->parameter_space_verticies_list[obj_file ->num_parameter_space_verticies].w = w;
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
process_new_texture_coords (ObjFile_t *obj_file, const char *input)
{
  if (obj_file->max_num_texture_coords == 0)
    {
      if (create_list ((void **)&obj_file->texture_coords_list,
                       sizeof (struct TextureCoord_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating texture coordinates list.\n");
          return -1;
        }

      obj_file->max_num_texture_coords = 1;
    }

  if (obj_file->num_texture_coords == obj_file->max_num_texture_coords)
    {
      obj_file->max_num_texture_coords *= 2;
      if (resize_list ((void **)&obj_file->texture_coords_list,
                       sizeof (struct TextureCoord_s)
                           * obj_file->max_num_texture_coords)
          != 0)
        {
          LOG_ERROR ("Error resizing texture coordinates list to size %ld.\n",
                     obj_file->max_num_texture_coords);
          return -1;
        }
    }

  float u, v = 0.0, w = 0.0;
  if (sscanf (input, "vt %f %f %f", &u, &v, &w) >= 1)
    {
      obj_file->texture_coords_list[obj_file->num_texture_coords].u = u;
      obj_file->texture_coords_list[obj_file->num_texture_coords].v = v;
      obj_file->texture_coords_list[obj_file->num_texture_coords].w = w;

      LOG_DEBUG_INFO ("vt %f %f %f\n", u, v, w);
    }
  else
    {
      LOG_ERROR ("Error parsing input: '%s'.\n", input);
      return -1;
    }

  obj_file->num_texture_coords++;
  return 0;
}

static int
process_new_vertex_coordinates (ObjFile_t *obj_file, const char *line)
{
  if (obj_file->max_num_verticies == 0)
    {
      if (create_list ((void **)&obj_file->verticies_list,
                       sizeof (struct VertexCoord_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating verticies list.\n");
          return -1;
        }

      obj_file->max_num_verticies = 1;
    }

  if (obj_file->num_verticies == obj_file->max_num_verticies)
    {
      obj_file->max_num_verticies *= 2;
      if (resize_list ((void **)&obj_file->verticies_list,
                       sizeof (struct VertexCoord_s)
                           * obj_file->max_num_verticies)
          != 0)
        {
          LOG_ERROR ("Error resizing vertices list to size %ld.\n",
                     obj_file->max_num_verticies);
          return -1;
        }
    }

  float x, y, z, w = 1.0;
  if (sscanf (line, "v %f %f %f %f", &x, &y, &z, &w) >= 3)
    {
      obj_file->verticies_list[obj_file->num_verticies].x = x;
      obj_file->verticies_list[obj_file->num_verticies].y = y;
      obj_file->verticies_list[obj_file->num_verticies].z = z;
      obj_file->verticies_list[obj_file->num_verticies].w = w;

      LOG_DEBUG_INFO ("v %f %f %f %f\n", x, y, z, w);

      obj_file->num_verticies++;
    }
  else
    {
      LOG_ERROR ("Error parsing line: '%s'.\n", line);
      return -1;
    }

  return 0;
}

static int
process_new_face (ObjFile_t *obj_file, const char *input)
{
  if (obj_file->max_num_faces == 0)
    {
      if (create_list ((void **)&obj_file->faces_list,
                       sizeof (struct PolygonalFace_s))
          != 0)
        {
          LOG_ERROR_MSG ("Error creating faces list.\n");
          return -1;
        }

      obj_file->max_num_faces = 1;
    }

  if (obj_file->num_faces == obj_file->max_num_faces)
    {
      obj_file->max_num_faces *= 2;
      if (resize_list ((void **)&obj_file->faces_list,
                       sizeof (struct PolygonalFace_s)
                           * obj_file->max_num_faces)
          != 0)
        {
          LOG_ERROR ("Error resizing faces list to size %ld.\n",
                     obj_file->max_num_faces);
          return -1;
        }
    }

  struct PolygonalFace_s *face = &obj_file->faces_list[obj_file->num_faces];
  ssize_t *v_sublist = face->vertices;
  ssize_t *t_sublist = face->texture_coords;
  ssize_t *n_sublist = face->vertex_normals;

  LOG_DEBUG_INFO ("%s", input);

  char input_cpy[256];
  strncpy (input_cpy, input, 255);
  input_cpy[strlen (input_cpy) - 1] = ' '; // Remove '\n'

  size_t num_elements = 0;
  char *input_ptr = input_cpy + 2;
  char *token = strchr (input_ptr, ' ');
  while (token != NULL)
    {
      *token = '\0';
      token++;

      size_t num_parts = 0;
      char *idx = strtok (input_ptr, "/");
      while (idx != NULL)
        {
          if (num_parts == 0)
            {
#ifdef DEBUG_BUILD
              assert ((size_t)(atoi (idx) - 1) < obj_file->num_verticies);
#endif /* DEBUG_BUILD */
              v_sublist[num_elements] = atoi (idx) - 1;
            }
          else if (num_parts == 1)
            {
#ifdef DEBUG_BUILD
              assert ((size_t)(atoi (idx) - 1) < obj_file->num_texture_coords);
#endif /* DEBUG_BUILD */
              t_sublist[num_elements] = atoi (idx) - 1;
            }
          else if (num_parts == 2)
            {
#ifdef DEBUG_BUILD
              assert ((size_t)(atoi (idx) - 1) < obj_file->num_vertex_normals);
#endif /* DEBUG_BUILD */
              n_sublist[num_elements] = atoi (idx) - 1;
            }

          num_parts++;
          /** If optional texture coords not provided, skip. */
          if ((idx + strlen (idx) + 1)[0] == '/')
            num_parts++;
          idx = strtok (NULL, "/");
        }

      input_ptr = token;
      num_elements++;
      token = strchr (input_ptr, ' ');
    }

  face->num_elements = num_elements;
  obj_file->num_faces++;
  return 0;
}

static int
read_obj_from_file (ObjFile_t obj_file[static 1], const char path[static 1])
{
  FILE *fptr = fopen (path, "r");
  if (fptr == NULL)
    {
      LOG_ERROR ("Unable to open file, %s.\n", path);
      return -1;
    }

  memset (obj_file, 0, sizeof (ObjFile_t));

  char line[256];
  while (fgets (line, sizeof (line), fptr) != NULL)
    {
      if (line[0] == '#') // ignore comments
        continue;
      else if (strncmp (line, "vn", 2) == 0)
        {
          if (process_new_vertex_normal (obj_file, line) != 0)
            goto clean_up;
        }
      else if (strncmp (line, "vp", 2) == 0)
        {
          if (process_new_parameter_space_vertex (obj_file, line) != 0)
            goto clean_up;
        }
      else if (strncmp (line, "vt", 2) == 0)
        {
          if (process_new_texture_coords (obj_file, line) != 0)
            goto clean_up;
        }
      else
        {
          switch (line[0])
            {
            case 'v':
              if (process_new_vertex_coordinates (obj_file, line) != 0)
                goto clean_up;
              break;
            case 'f':
              if (process_new_face (obj_file, line) != 0)
                goto clean_up;
              break;
            case 'o':
            case 'l':
            default:
              LOG_DEBUG_INFO ("Ignoring: %s", line);
              break;
            }
        }
    }

  fclose (fptr);
  return 0;
clean_up:
  fclose (fptr);
  return -1;
}

int
create_obj_file_from_file (ObjFile_t o[static 1],
                           const char file_path[static 1])
{
  return read_obj_from_file (o, file_path);
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
