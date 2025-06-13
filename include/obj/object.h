#ifndef _WAVEFRONT_OBJ_OBJECT_H_
#define _WAVEFRONT_OBJ_OBJECT_H_

#include "dynamic_arr.h"

#include <stdio.h>

typedef struct NamedObject_s NamedObject_t;
extern const size_t NO_SIZE_BYTES;

NamedObject_t *no_alloc (void);
void no_free (NamedObject_t *no);
int no_init (NamedObject_t *no, const char name[static 1],
             FILE input_fptr[static 1], DynamicArray_t *verts,
             DynamicArray_t *norms);
void no_print (void *no);

#endif /* _WAVEFRONT_OBJ_OBJECT_H_ */
