#ifndef _WAVEFRONT_OBJ_VERTEX_COORDINATE_H_
#define _WAVEFRONT_OBJ_VERTEX_COORDINATE_H_

#include <stddef.h>

typedef struct _OBJVertexCoord_s _OBJVertexCoord_t;

_OBJVertexCoord_t *objv_alloc (char input[static 1]);
void objv_free (_OBJVertexCoord_t *v);

/** Returns the size in bytes of `_OBJVertexCoord_t`. */
size_t objv_size (void);

float objv_get_x (_OBJVertexCoord_t *v);
float objv_get_y (_OBJVertexCoord_t *v);
float objv_get_z (_OBJVertexCoord_t *v);

#endif /* _WAVEFRONT_OBJ_VERTEX_COORDINATE_H_ */
