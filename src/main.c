/**
 *  NOTE: This whole project is an experiment. Things might look a little rough
 *  while I'm figuring everything out, but I'll come back and clean it all back
 *  up at some later point.
 */
#include "obj/file.h"

#include <stdlib.h>

int
main (void)
{
  WavefrontObj_t *obj = obj_alloc ();
  if (obj == NULL)
    return EXIT_FAILURE;

  int ret = obj_init (obj, "test.obj");
  obj_free (obj);

  return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
