#include "obj/file.h"
#include "dyna.h"
#include "obj/errors.h"
#include "obj/obj.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ObjFile_s
{
  char *path;
  DynamicArr_t *objs;
};

static int
process_object (ObjFile_t *o, FILE *fptr, char *input)
{
  _OBJObj_t *obj = objo_alloc (input, fptr);
  if (!obj)
    return -1;

  if (DynA_append (o->objs, obj) != 0)
    {
      objo_free (obj);
      return -1;
    }

  objo_free (obj);
  return 0;
}

int
process_file (ObjFile_t o[static 1])
{
  FILE *fptr = fopen (o->path, "r");
  if (!fptr)
    {
      fprintf (stderr, "%s: Error opening file: %s\n", __func__, o->path);
      return -1;
    }

  char *l = NULL;
  size_t l_size = 0;
  while ((getline (&l, &l_size, fptr)) != -1)
    {
      if (l_size == 0)
        break;
      else if (l[0] == '#')
        continue;
      else if (strncmp (l, "o ", 2) == 0)
        {
          if (process_object (o, fptr, l + 2) != 0)
            goto loop_err_exit;
        }
      else
        {
          fprintf (stderr, "Malformed input: %s", l);
          goto loop_err_exit;
        }

      free (l);
      l = NULL;
      l_size = 0;
    }

  if (l)
    free (l);
  fclose (fptr);
  return 0;
loop_err_exit:
  free (l);
  fclose (fptr);
  return -1;
}

ObjFile_t *
obj_alloc (char path[static 1])
{
  ObjFile_t *o = calloc (1, sizeof (ObjFile_t));
  if (!o)
    {
      WAVOBJ_OOM_ERR ();
      return NULL;
    }

  o->objs = DynA_alloc (objo_size ());
  if (!o->objs)
    {
      WAVOBJ_OOM_ERR ();
      obj_free (o);
      o = NULL;
      return NULL;
    }

  o->path = strdup (path);
  if (!o->path)
    {
      WAVOBJ_OOM_ERR ();
      obj_free (o);
      o = NULL;
      return NULL;
    }

  if (process_file (o) != 0)
    {
      obj_free (o);
      o = NULL;
      return NULL;
    }

  return o;
}

void
obj_free (ObjFile_t *o)
{
  if (o == NULL)
    return;

  if (o->path)
    free (o->path);
  o->path = NULL;

  if (o->objs)
    DynA_free (o->objs);
  o->objs = NULL;

  free (o);
  o = NULL;
}
