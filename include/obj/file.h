#ifndef _WAVEFRONT_OBJ_FILE_H_
#define _WAVEFRONT_OBJ_FILE_H_

#include "dyna.h"

typedef struct OBJFile_s OBJFile_t;

OBJFile_t *obj_alloc (char path[static 1]);
void obj_free (OBJFile_t *o);

/**
 *  Returns a ptr to a dynamic array which contains every `_OBJObj_t` in the
 *  associated obj file. Returns NULL when `o` or `o->objs` is NULL.
 */
DynamicArr_t *obj_get_objs (OBJFile_t *o);

#endif /* _WAVEFRONT_OBJ_FILE_H_ */
