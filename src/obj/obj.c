#include "obj/obj.h"
#include "dyna.h"
#include "obj/errors.h"
#include "obj/vertex_coord.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _OBJObj_s
{
  char *name;
  DynamicArr_t *verts;
};

static int
process_vertex_coord (_OBJObj_t *o, char *l)
{
  float vals[3] = { 0 };
  size_t i = 0;
  for (char *ptr = l; ptr && i < 3; i++)
    {
      char *end = strchr (ptr, ' ');
      if (end)
        *end = '\0';
      vals[i] = atof (ptr);
      if (end)
        ptr = end + 1;
    }

  _OBJVertexCoord_t v = { .x = vals[0], .y = vals[1], .z = vals[2] };
  if (DynA_append (o->verts, &v) != 0)
    {
      return -1;
    }

  return 0;
}

static int
process_file (_OBJObj_t o[static 1], FILE fptr[static 1])
{
  char *l = NULL;
  size_t l_len = 0;
  while ((getline (&l, &l_len, fptr)) != -1)
    {
      if (l_len == 0)
        {
          if (l)
            free (l);
          l = NULL;
          break;
        }
      else if (l[0] == '#')
        goto loop_end;
      else if (strncmp (l, "v ", 2) == 0)
        {
          if (process_vertex_coord (o, l + 2) != 0)
            {
              free (l);
              l = NULL;
              return -1;
            }
        }
      else if (strncmp (l, "vt ", 3) == 0) // ignoring text coords for now
        goto loop_end;
      else
        {
          printf ("%s", l);
          free (l);
          l = NULL;
          break;
        }

    loop_end:
      free (l);
      l = NULL;
    }

  return 0;
}

_OBJObj_t *
objo_alloc (char name[static 1], FILE fptr[static 1])
{
  _OBJObj_t *o = calloc (1, sizeof (_OBJObj_t));
  if (!o)
    {
      WAVOBJ_OOM_ERR ();
      return NULL;
    }

  o->name = strdup (name);
  if (!o->name)
    goto alloc_err_exit;

  // remove '\n' if present
  size_t name_len = strlen (o->name) - 1;
  if (o->name[name_len] == '\n')
    o->name[name_len] = '\0';

  o->verts = DynA_alloc (sizeof (_OBJVertexCoord_t));
  if (!o->verts)
    goto alloc_err_exit;

  if (process_file (o, fptr) != 0)
    goto alloc_err_exit;

  return o;
alloc_err_exit:
  WAVOBJ_OOM_ERR ();
  objo_free (o);
  return NULL;
}

void
objo_free (_OBJObj_t *o)
{
  if (!o)
    return;

  if (o->name)
    free (o->name);
  o->name = NULL;

  if (o->verts)
    DynA_free (o->verts);
  o->verts = NULL;

  free (o);
}

size_t
objo_size (void)
{
  return sizeof (_OBJObj_t);
}
