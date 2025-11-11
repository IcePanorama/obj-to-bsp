#include "obj/face.h"
#include "dyna.h"
#include "obj/errors.h"
#include "obj/vertex_coord.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_OBJFace_t *
objf_alloc (char input[static 1], struct _DynamicArr_s *verts)
{
  _OBJFace_t *f = calloc (1, sizeof (_OBJFace_t));
  char *input_cpy = strdup (input); // for an error message later.
  if ((!f) || (!input_cpy))
    {
      WAVOBJ_OOM_ERR ();

      if (f)
        objf_free (f);
      if (input_cpy)
        free (input_cpy);

      return NULL;
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
          objf_free (f);
          return NULL;
        }

      _OBJVertexCoord_t *v = DynA_at (verts, atoi (tok) - 1);
      if (!v)
        {
          // fixme: redo later lol
          fprintf (stderr, "%s: shit's fucked! f %s\n", __func__, input_cpy);
          free (input_cpy);
          objf_free (f);
          return NULL;
        }

      f->vertices[i][0] = objv_get_x (v);
      f->vertices[i][1] = objv_get_y (v);
      f->vertices[i][2] = objv_get_z (v);

      if (end)
        ptr = end + 1;
    }

  free (input_cpy);
  return f;
}

void
objf_free (_OBJFace_t *f)
{
  if (!f)
    return;

  free (f);
  f = NULL;
}

size_t
objf_size (void)
{
  return sizeof (_OBJFace_t);
}

size_t *
objf_get_vert_idxs (_OBJFace_t *f)
{
  if (!f)
    return NULL;

  return NULL;
}
