#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct _DynamicArray_s DynamicArray_t;

DynamicArray_t *dyna_create (size_t el_size);
void dyna_free (DynamicArray_t *arr);
/** Returns:  zero on success, non-zero on failure. */
int dyna_resize (DynamicArray_t *arr, size_t new_capacity);
/** Returns:  zero on success, non-zero on failure. */
int dyna_append (DynamicArray_t *arr, void *el);

#endif /* _DYNAMIC_ARRAY_H_ */
