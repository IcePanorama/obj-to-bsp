#include "obj/file.h"
#include "obj/object.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct WavefrontObj_s
{
  char *filename;
};

WavefrontObj_t *
obj_alloc (void)
{
  return calloc (1, sizeof (WavefrontObj_t));
}

void
obj_free (WavefrontObj_t *obj)
{
  if (obj == NULL)
    return;

  if (obj->filename != NULL)
    free (obj->filename);

  free (obj);
}

static int
append_object (WavefrontObj_t *obj, const char name[static 1],
               FILE *input_fptr)
{
  NamedObject_t *o = no_alloc ();
  if (o == NULL)
    {
      fprintf (stderr, "%s: Out of memory error.\n", __func__);
      return -1;
    }

  if (no_init (o, name, input_fptr) != 0)
    {
      no_free (o);
      return -1;
    }

  no_print (o);
  no_free (o);
  return 0;
  obj_free (obj);
}

int
obj_init (WavefrontObj_t *obj, const char path[static 1])
{
  if (obj == NULL)
    return -1;

  // +1 for NULL terminator
  obj->filename = malloc (sizeof (char) * (strlen (path) + 1));
  if (obj->filename == NULL)
    {
      fprintf (stderr, "%s: Out of memory error.\n", __func__);
      return -1;
    }
  strcpy (obj->filename, path);

  FILE *obj_fptr = fopen (obj->filename, "r");
  if (obj_fptr == NULL)
    return EXIT_FAILURE;

  while (!feof (obj_fptr))
    {
      char line[256];
      if (fgets (line, 256, obj_fptr) == NULL)
        goto err_exit;
      switch (line[0])
        {
        case '#':
          continue;
        case 'o':
          if (append_object (obj, line + 2, obj_fptr) != 0)
            goto err_exit;
          break;
        default:
          fprintf (stderr, "%s: Unrecognized char, '%c', in line %s", __func__,
                   line[0], line);
          goto err_exit;
        }
    }

  fclose (obj_fptr);
  return 0;
err_exit:
  fclose (obj_fptr);
  return -1;
}
