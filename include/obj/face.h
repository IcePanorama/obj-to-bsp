#ifndef _WAVEFRONT_OBJ_POLYGONAL_FACE_H_
#define _WAVEFRONT_OBJ_POLYGONAL_FACE_H_

#include <stddef.h>

struct _DynamicArr_s;

typedef struct _OBJFace_s
{
  float vertices[3][3];
} OBJFace_t;

int objf_init (OBJFace_t f[static 1], char input[static 1],
               struct _DynamicArr_s *verts);

#endif /* _WAVEFRONT_OBJ_POLYGONAL_FACE_H_ */
