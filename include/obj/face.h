#ifndef _WAVEFRONT_OBJ_POLYGONAL_FACE_H_
#define _WAVEFRONT_OBJ_POLYGONAL_FACE_H_

#include <stddef.h>

struct _DynamicArr_s;

typedef struct _OBJFace_s
{
  float vertices[3][3];
} _OBJFace_t;

int objf_init (_OBJFace_t f[static 1], char input[static 1],
               struct _DynamicArr_s *verts);

/** Returns the size of `_OBJFace_t` in bytes. */
size_t objf_size (void);

#endif /* _WAVEFRONT_OBJ_POLYGONAL_FACE_H_ */
