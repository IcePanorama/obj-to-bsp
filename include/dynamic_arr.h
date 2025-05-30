#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include <stdlib.h>

typedef struct DynamicArray_s DynamicArray_t;

DynamicArray_t *dyna_alloc (size_t el_size);

void dyna_free (DynamicArray_t *arr);

/** Returns:  zero on success, non-zero on failure. */
int dyna_resize (DynamicArray_t *arr, size_t new_capacity);

/** Returns:  zero on success, non-zero on failure. */
int dyna_append (DynamicArray_t *arr, void *el);

size_t dyna_get_size (DynamicArray_t *arr);

void *dyna_at (DynamicArray_t *arr, size_t idx);

/** Prints each element in `arr` by calling `print_fn`. */
void dyna_print (DynamicArray_t *arr, void (*print_fn) (void *el));

#endif /* _DYNAMIC_ARRAY_H_ */
