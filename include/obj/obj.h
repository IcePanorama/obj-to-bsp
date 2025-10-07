/** obj.h - Wavefront OBJ objects. Intended for internal usage only. */
#ifndef _WAVEFRONT_OBJ_OBJECTS_H_
#define _WAVEFRONT_OBJ_OBJECTS_H_

#include "dyna.h"

#include <stdio.h>

typedef struct _OBJObj_s _OBJObj_t;

_OBJObj_t *objo_alloc (char name[static 1], FILE fptr[static 1]);
void objo_free (_OBJObj_t *o);

/** Returns the size in bytes of `_OBJObj_t`. */
size_t objo_size (void);

DynamicArr_t *objo_get_verts (_OBJObj_t *o);
DynamicArr_t *objo_get_faces (_OBJObj_t *o);

#endif /* _WAVEFRONT_OBJ_OBJECTS_H_ */
