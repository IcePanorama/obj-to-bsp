#include "obj/obj.h"
#include "dyna.h"
#include "obj/errors.h"
#include "obj/face.h"
#include "obj/vertex.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _OBJObj_s
{
  char *name;
  DynamicArr_t *faces;
};

static int
process_vertex (DynamicArr_t *verts, char l[static 1])
{
  OBJVertexCoord_t v = { 0 };
  if (objv_init (&v, l) != 0)
    return -1;

  if (DynA_append (verts, &v) != 0)
    return -1;

  return 0;
}

static int
process_face (OBJObj_t *o, char *l, DynamicArr_t *verts)
{
  OBJFace_t f = { 0 };
  if (objf_init (&f, l, verts) != 0)
    return -1;

  if (DynA_append (o->faces, &f) != 0)
    {
      return -1;
    }

  return 0;
}

static bool
peek_next_char_is (FILE *fptr, char exp)
{
  char act = fgetc (fptr);
  bool ret = act == exp;
  ungetc (act, fptr);
  return ret;
}

static int
process_file (OBJObj_t o[static 1], FILE fptr[static 1])
{
  char *l = NULL;
  size_t l_len = 0;

  DynamicArr_t *verts = DynA_alloc (sizeof (OBJVertexCoord_t));
  if (!verts)
    {
      fprintf (stderr, "[%s] Out of memory error!\n", __func__);
      return -1;
    }

  // Need to make sure we don't "get" the first line of another object.
  while (!(peek_next_char_is (fptr, 'o'))
         && ((getline (&l, &l_len, fptr)) != -1))
    {
      if (l_len == 0)
        break;
      // ignoring comments, tex coords, shading
      else if ((l[0] == '#') || (strncmp (l, "s ", 2) == 0)
               || (strncmp (l, "vt ", 3) == 0))
        {
          goto loop_end;
        }

      if (strncmp (l, "f ", 2) == 0)
        {
          if (process_face (o, l + 2, verts) != 0)
            goto err_exit;
        }
      else if (strncmp (l, "v ", 2) == 0)
        {
          if (process_vertex (verts, l + 2) != 0)
            goto err_exit;
        }
      else
        {
          printf ("%s", l);
          break;
        }

    loop_end:
      free (l);
      l = NULL;
      l_len = 0;
    }

  if (l)
    free (l);
  DynA_free (verts);
  return 0;
err_exit:
  free (l);
  DynA_free (verts);
  return -1;
}

OBJObj_t *
objo_alloc (char name[static 1], FILE fptr[static 1])
{
  OBJObj_t *o = calloc (1, sizeof (OBJObj_t));
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

  o->faces = DynA_alloc (sizeof (OBJFace_t));
  if (!o->faces)
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
objo_free (OBJObj_t *o)
{
  if (!o)
    return;

  if (o->name)
    free (o->name);
  o->name = NULL;

  if (o->faces)
    DynA_free (o->faces);
  o->faces = NULL;

  free (o);
  o = NULL;
}

size_t
objo_size (void)
{
  return sizeof (OBJObj_t);
}

DynamicArr_t *
objo_get_faces (OBJObj_t *o)
{
  if ((!o) || (!o->faces))
    return NULL;

  return o->faces;
}
