#include "obj/object.h"
#include "dynamic_arr.h"
#include "obj/normal.h"
#include "obj/vert_coords.h"
#include "vec3.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct NamedObject_s
{
  char *name;
  // Face_t *faces;
};

NamedObject_t *
no_alloc (void)
{
  return calloc (1, sizeof (NamedObject_t));
}

void
no_free (NamedObject_t *no)
{
  if (no == NULL)
    return;

  if (no->name != NULL)
    free (no->name);
  free (no);
}

int
no_init (NamedObject_t *no, const char name[static 1],
         FILE input_fptr[static 1])
{
  if (no == NULL)
    return -1;

  no->name = malloc (sizeof (char) * (strlen (name) + 1));
  if (no->name == NULL)
    return -1;
  strcpy (no->name, name);
  no->name[strlen (no->name) - 1] = '\0'; // replace '\n' w/ '\0'

  DynamicArray_t *verts = dyna_alloc (sizeof (VertCoord_t));
  DynamicArray_t *norms = dyna_alloc (sizeof (Normal_t));
  if ((verts == NULL) || (norms == NULL))
    {
      fprintf (stderr, "%s: Out of memory error.\n", __func__);
      if (verts != NULL)
        free (verts);
      if (norms != NULL)
        free (norms);
      return -1;
    }

  while (!feof (input_fptr))
    {
      char curr_line[256] = { 0 };
      if (fgets (curr_line, 256, input_fptr) == NULL)
        {
          fprintf (stderr, "%s: failed to read line from file.\n", __func__);
          goto err_exit;
        }

      if (strncmp (curr_line, "vn", 2) == 0)
        {
          Normal_t n = nm_init (curr_line + 3);
          if (dyna_append (norms, &n) != 0)
            goto append_failure;
        }
      else if (strncmp (curr_line, "vt", 2) == 0)
        {
          printf ("%s", curr_line + 3);
          break;
        }
      else if (curr_line[0] == 'v')
        {
          VertCoord_t v = vc_init (curr_line + 2);
          if (dyna_append (verts, (void *)&v) != 0)
            goto append_failure;
          /*
                {
                  // FIXME: clean up
                  fprintf (stderr, "%s: append failed.\n", __func__);
                  goto err_exit;
                }
                */
        }
      else
        {
          fprintf (stderr, "%s: Unrecognized line %s", __func__, curr_line);
          goto err_exit;
        }
    }

  dyna_free (norms);
  dyna_free (verts);
  return 0;
append_failure:
  fprintf (stderr, "%s: append failed.\n", __func__);
err_exit:

  puts ("Verts:");
  dyna_print (verts, &v3_print);
  puts ("Norms:");
  dyna_print (norms, &v3_print);

  dyna_free (norms);
  dyna_free (verts);
  return -1;
}

void
no_print (NamedObject_t *no)
{
  printf ("Name: %s\n", no->name);
}
