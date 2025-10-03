#include "obj/face.h"
#include "obj/errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _OBJFace_s
{
  size_t vert_idx[3];
};

_OBJFace_t *
objf_alloc (char input[static 1])
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

      f->vert_idx[i] = atoi (tok) - 1;

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
