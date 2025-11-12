#include "obj/face.h"
#include "dyna.h"
#include "obj/errors.h"
#include "obj/vertex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
objf_init (OBJFace_t f[static 1], char input[static 1],
           struct _DynamicArr_s *verts)
{
  char *input_cpy = strdup (input); // for an error message later.
  if (!input_cpy)
    {
      WAVOBJ_OOM_ERR ();
      return -1;
    }

  size_t i = 0;
  for (char *ptr = input; ptr && i < 3; i++)
    {
      char *end = strchr (ptr, ' ');
      if (end)
        *end = '\0';

      char *tok = strtok (ptr, "/"); // don't care about vt/vn, ignoring those
      if (!tok)
        {
          fprintf (stderr, "%s: Malformed input with line: f %s\n", __func__,
                   input_cpy);
          free (input_cpy);
          return -1;
        }

      _OBJVertexCoord_t *v = DynA_at (verts, atoi (tok) - 1);
      if (!v)
        {
          // fixme: redo later lol
          fprintf (stderr, "%s: shit's fucked! f %s\n", __func__, input_cpy);
          free (input_cpy);
          return -1;
        }

      f->vertices[i][0] = v->x;
      f->vertices[i][1] = v->y;
      f->vertices[i][2] = v->z;

      if (end)
        ptr = end + 1;
    }

  free (input_cpy);
  return 0;
}
