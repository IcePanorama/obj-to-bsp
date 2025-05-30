#include "obj/vert_coords.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

VertCoord_t
vc_init (char *raw_data)
{
  float coords[3] = { 0 };
  size_t coord_idx = 0;

  char *tok = strtok (raw_data, " ");
  while (tok != NULL)
    {
      assert (coord_idx < 3);
      coords[coord_idx] = strtof (tok, NULL);
      coord_idx++;
      tok = strtok (NULL, " ");
    }

  return (VertCoord_t){ .x = coords[0], .y = coords[1], .z = coords[2] };
}
