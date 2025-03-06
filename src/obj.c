#include "obj.h"

#include <stdlib.h>

void
free_obj (ObjFile_t *o)
{
  if (o->verticies_list != NULL)
    free (o->verticies_list);
  if (o->vertex_normals_list != NULL)
    free (o->vertex_normals_list);
  if (o->parameter_space_verticies_list != NULL)
    free (o->parameter_space_verticies_list);
  if (o->texture_coords_list != NULL)
    free (o->texture_coords_list);
}
