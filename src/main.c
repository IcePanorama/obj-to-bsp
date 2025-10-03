#include "bsp/tree.h"
#include "obj/file.h"

#include <stdio.h>
#include <stdlib.h>

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
    return EXIT_FAILURE;

  BSPTree_t *t = bsp_alloc (obj);
  if (!t)
    {
      obj_free (obj);
      return EXIT_FAILURE;
    }

  obj_free (obj);
  return EXIT_SUCCESS;
}
