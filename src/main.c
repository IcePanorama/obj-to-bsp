#include "bsp/tree.h"
#include "obj/file.h"
#include "obj/obj.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      // TODO: make usage cmd line arg
      fprintf (stderr, "Improper usage error.\n\ttry: %s path/to/file.obj\n",
               argv[0]);
      return EXIT_FAILURE;
    }

  OBJFile_t *obj = obj_alloc (argv[1]);
  if (!obj)
    {
      fprintf (stderr, "obj_alloc failure\n");
      return EXIT_FAILURE;
    }

  BSPTree_t *t = bsp_alloc (*(OBJObj_t **)DynA_at (obj_get_objs (obj), 0));
  /*
  if (!t)
    {
      fprintf (stderr, "bsp_alloc failure\n");
      obj_free (obj);
      return EXIT_FAILURE;
    }
    */

  if (t)
    bsp_free (t);

  obj_free (obj);
  return EXIT_SUCCESS;
}
