#include "obj/file.h"
#include "dynamic_arr.h"
#include "obj/normal.h"
#include "obj/object.h"
#include "obj/vert_coords.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct WavefrontObj_s
{
  char *filename;
  DynamicArray_t *verts;
  DynamicArray_t *norms;
  DynamicArray_t *objs;
};

WavefrontObj_t *
obj_alloc (void)
{
  WavefrontObj_t *out = calloc (1, sizeof (WavefrontObj_t));
  if (out == NULL)
    return NULL;

  out->verts = dyna_alloc (sizeof (VertCoord_t));
  out->norms = dyna_alloc (sizeof (Normal_t));
  out->objs = dyna_alloc (NO_SIZE_BYTES);
  if ((out->verts == NULL) || (out->norms == NULL) || (out->objs == NULL))
    {
      obj_free (out);
      return NULL;
    }

  return out;
}

void
obj_free (WavefrontObj_t *obj)
{
  if (obj == NULL)
    return;

  if (obj->filename != NULL)
    free (obj->filename);

  if (obj->verts != NULL)
    dyna_free (obj->verts);
  if (obj->norms != NULL)
    dyna_free (obj->norms);
  if (obj->objs != NULL)
    dyna_free (obj->objs);

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

  if (no_init (o, name, input_fptr, obj->verts, obj->norms) != 0)
    goto err_exit;

  if (dyna_append (obj->objs, o) != 0)
    {
      fprintf (stderr, "Error appending named object to file %s.\n",
               obj->filename);
      goto err_exit;
    }

  no_free (o);
  return 0;
err_exit:
  no_free (o);
  return -1;
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
        case '#': // Skip comments
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

  obj_print (obj);

  fclose (obj_fptr);
  return 0;
err_exit:
  fclose (obj_fptr);
  return -1;
}

void
obj_print (WavefrontObj_t *obj)
{
  if (obj == NULL)
    return;

  printf ("Wavefront object file: %s\n", obj->filename);
  printf ("%zu objects, %zu vertices, %zu normals\n",
          dyna_get_size (obj->objs), dyna_get_size (obj->verts),
          dyna_get_size (obj->norms));
  dyna_print (obj->objs, no_print);
}
