#include "obj/face.h"
#include "dynamic_arr.h"
#include "obj/normal.h"
#include "obj/vert_coords.h"
#include "vec3.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Face_t
fc_init (char raw_data[static 1], DynamicArray_t *verts, DynamicArray_t *norms)
{
  assert (verts != NULL);
  assert (norms != NULL);
  Face_t f = { 0 };

  printf ("Raw: %s", raw_data);
  size_t curr_vert = 0;
  size_t curr_norm = 0;
  char *tok = strtok (raw_data, " \n");
  while (tok != NULL)
    {
      assert (('0' <= tok[0]) && (tok[0] <= '9'));
      char *div = strchr (tok, '/');
      assert (div != NULL);
      *div = '\0';
      size_t vert_idx = atoi (tok) - 1; // Obj files use 1-indexing
      memcpy (&f.vertices[curr_vert], dyna_at (verts, vert_idx),
              sizeof (VertCoord_t));
      curr_vert++;

      // FIXME: currently not handling texture coordinates.
      tok = div + 1;
      do
        {
          tok++;
        }
      while ((tok != NULL) && (*tok == '/'));

      assert (('0' <= tok[0]) && (tok[0] <= '9'));
      size_t norm_idx = atoi (tok) - 1;
      memcpy (&f.normals[curr_norm], dyna_at (norms, norm_idx),
              sizeof (Normal_t));
      curr_norm++;
      tok = strtok (NULL, " \n");
    }

  f.num_points = curr_vert;
  return f;
}

void
fc_print (Face_t f[static 1])
{
  printf ("Tri or quad? %s\n", (f->num_points == 3) ? "tri" : "quad");
  puts ("Vertices:");
  for (size_t i = 0; i < f->num_points; i++)
    {
      v3_print (&f->vertices[i]);
    }
  puts ("Normals");
  for (size_t i = 0; i < f->num_points; i++)
    {
      v3_print (&f->normals[i]);
    }
}
