#ifndef _WAVEFRONT_OBJ_POLYGONAL_FACE_H_
#define _WAVEFRONT_OBJ_POLYGONAL_FACE_H_

#include <stddef.h>

typedef struct _OBJFace_s _OBJFace_t;

_OBJFace_t *objf_alloc (char input[static 1]);
void objf_free (_OBJFace_t *f);

/** Returns the size of `_OBJFace_t` in bytes. */
size_t objf_size (void);

#endif /* _WAVEFRONT_OBJ_POLYGONAL_FACE_H_ */
