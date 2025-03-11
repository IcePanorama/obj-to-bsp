#include "covar.h"
#include "log.h"
#include "obj.h"

#include <stdlib.h>

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
  ObjFile_t obj;
  int status = create_obj_file_from_file (&obj, argv[1]);

  struct VertexCoord_s centroid = calc_centroid_from_obj (&obj);
  LOG_DEBUG_INFO ("Centroid: (%f, %f, %f, %f)\n", centroid.x, centroid.y,
                  centroid.z, centroid.w);

  float covar_mat[4][4] = { 0 };
  calc_covar_mat_from_obj_centroid (&obj, &centroid, covar_mat);
  print_covar_mat (covar_mat);

  free_obj_file (&obj);
  return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
