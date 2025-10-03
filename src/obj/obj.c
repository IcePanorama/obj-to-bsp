#include "obj/obj.h"
#include "dyna.h"
#include "obj/errors.h"
#include "obj/face.h"
#include "obj/vertex_coord.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _OBJObj_s
{
  char *name;
  DynamicArr_t *verts;
  DynamicArr_t *faces;
};

static int
process_vertex_coord (_OBJObj_t *o, char *l)
{
  _OBJVertexCoord_t *v = objv_alloc (l);
  if (!v)
    return -1;

  if (DynA_append (o->verts, &v) != 0)
    {
      objv_free (v);
      return -1;
    }

  objv_free (v);
  return 0;
}

static int
process_face (_OBJObj_t *o, char *l)
{
  _OBJFace_t *f = objf_alloc (l);
  if (!f)
    return -1;

  if (DynA_append (o->faces, f) != 0)
    {
      objf_free (f);
      return -1;
    }

  objf_free (f);
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
        break;
      // ignoring comments, tex coords, shading
      else if ((l[0] == '#') || (strncmp (l, "s ", 2) == 0)
               || (strncmp (l, "vt ", 3) == 0))
        {
          goto loop_end;
        }

      if (strncmp (l, "f ", 2) == 0)
        {
          if (process_face (o, l + 2) != 0)
            goto err_exit;
        }
      else if (strncmp (l, "v ", 2) == 0)
        {
          if (process_vertex_coord (o, l + 2) != 0)
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
  return 0;
err_exit:
  free (l);
  return -1;
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

  o->verts = DynA_alloc (objv_size ());
  o->faces = DynA_alloc (objf_size ());
  if ((!o->verts) || (!o->faces))
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

  if (o->faces)
    DynA_free (o->faces);
  o->faces = NULL;

  free (o);
  o = NULL;
}

size_t
objo_size (void)
{
  return sizeof (_OBJObj_t);
}
