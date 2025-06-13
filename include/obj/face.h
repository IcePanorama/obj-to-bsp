#ifndef _WAVEFRONT_OBJ_FACE_H_
#define _WAVEFRONT_OBJ_FACE_H_

#include "dynamic_arr.h"
#include "obj/normal.h"
#include "obj/vert_coords.h"

#include <stdlib.h>

typedef struct Face_s
{
  size_t num_points; // 3 for tri, 4 for quad.
  VertCoord_t vertices[4];
  Normal_t normals[4];
} Face_t;

Face_t fc_init (char raw_data[static 1], DynamicArray_t *verts,
                DynamicArray_t *norms);

void fc_print (Face_t f[static 1]);

#endif /* _WAVEFRONT_OBJ_FACE_H_ */
