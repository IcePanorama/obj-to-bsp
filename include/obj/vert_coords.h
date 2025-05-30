#ifndef _WAVEFRONT_OBJ_VERTEX_COORDINATE_H_
#define _WAVEFRONT_OBJ_VERTEX_COORDINATE_H_

#include "vec3.h"

typedef Vec3_t VertCoord_t;

VertCoord_t vc_init (char *raw_data);
void vc_print (VertCoord_t *v);

#endif /* _WAVEFRONT_OBJ_VERTEX_COORDINATE_H_ */
