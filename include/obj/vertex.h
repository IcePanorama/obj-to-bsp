#ifndef _WAVEFRONT_OBJ_VERTEX_COORDINATE_H_
#define _WAVEFRONT_OBJ_VERTEX_COORDINATE_H_

#include <stddef.h>

typedef struct _OBJVertexCoord_s
{
  float x;
  float y;
  float z;
  // float w; // optional, defaults to 1.0.
} OBJVertexCoord_t;

int objv_init (OBJVertexCoord_t v[static 1], char input[static 1]);

#endif /* _WAVEFRONT_OBJ_VERTEX_COORDINATE_H_ */
