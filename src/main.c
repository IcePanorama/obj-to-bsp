/**
 *  NOTE: This whole project is an experiment. Things might look a little rough
 *  while I'm figuring everything out, but I'll come back and clean it all back
 *  up at some later point.
 */
#include "bsp_tree/tree.h"
#include "log.h"
#include "obj/file.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      LOG_ERROR ("Improper usage error.\n\ttry: %s path/to/file.obj\n",
                 argv[0]);
      return EXIT_FAILURE;
    }

  // FIXME: assuming argv[1] is an obj file for the time being.
  ObjFile_t *obj = obj_create (argv[1]);
  if (obj == NULL)
    return EXIT_FAILURE;

  /**
   *  NOTE: The calculations required to traverse a tree split in this manner
   *  may be too expensive for platforms with constrained hardware. May be
   *  worth exploring other splitting methods in the future.
   *  A cheaper splitting plan
   *  may be to treat the normal of the splitting plane as some fixed value. We
   *  could build a BSP tree for each x-, y-, and z-axis aligned normal (as
   *  well as their negative counterparts), and then select the tree which is
   *  the shortest as our output.
   */
  float centroid[4] = { 0 };
  obj_calc_centroid (obj, centroid);
  LOG_DEBUG_INFO ("Centroid: (%f, %f, %f, %f)\n", centroid[0], centroid[1],
                  centroid[2], centroid[3]);

  float covar_mat[16] = { 0 };
  obj_calc_covar_mat_w_centroid (obj, centroid, covar_mat);

  print_covar_mat (covar_mat);

  float eval[4] = { 0 };
  float evec[4][4] = { { 0 } };
  if (calc_eigenvals_vecs (covar_mat, eval, evec) != 0)
    {
      obj_free (obj);
      return EXIT_FAILURE;
    }

  float *largest_evec;
  get_largest_evec_from_largest_eval (eval, evec, &largest_evec);

  char tmp[256] = { 0 };
  for (size_t i = 0; i < 4; i++)
    {
      char line[32] = { 0 };
      sprintf (line, "%f ", largest_evec[i]);
      strcat (tmp, line);
    }
  LOG_DEBUG_INFO ("Splitting plane normal: %s\n", tmp);

  BSPTree_t *bsp = bsp_alloc ();
  if (bsp == NULL)
    goto err_exit;

  if (bsp_process_obj (bsp, obj, centroid, largest_evec) != 0)
    {
      bsp_free (bsp);
      goto err_exit;
    }

  bsp_free (bsp);
  obj_free (obj);

  return EXIT_SUCCESS;
err_exit:
  obj_free (obj);
  return EXIT_FAILURE;
}
