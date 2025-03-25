#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct _DynamicArray_s DynamicArray_t;

DynamicArray_t *dyna_create (size_t el_size);
void dyna_free (DynamicArray_t *arr);
int dyna_append (DynamicArray_t *arr, void *el);

#endif /* _DYNAMIC_ARRAY_H_ */
