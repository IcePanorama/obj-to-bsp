/** dyna.h - a type-agnostic dynamic array C library. */
#ifndef _DYNA_DYNAMIC_ARRAY_LIBRARY_H_
#define _DYNA_DYNAMIC_ARRAY_LIBRARY_H_

#include <stddef.h>

typedef struct _DynamicArr_s DynamicArr_t;

/**
 *  An `el_size` of 0 is treated as an unhandled exception when NDEBUG is
 *  undefined, else this returns NULL. May return NULL upon calloc failure.
 *  Param:  el_size  Size of a single element in bytes.
 */
DynamicArr_t *DynA_alloc (size_t el_size);

/**
 *  When NDEBUG is undefined, a NULL `a` is treated like an unhandled
 *  exception, otherwise this will behave well upon receiving such input.
 */
void DynA_free (DynamicArr_t *a);

/**
 *  Returns the physical size of `a` in bytes. Returns 0 upon NULL input when
 *  NDEBUG is defined, otherwise NULL input is treated as an unhandled
 *  exception.
 */
size_t DynA_get_capacity (DynamicArr_t *a);

/**
 *  Returns the logical size of `a` in bytes. Returns 0 upon NULL input when
 *  NDEBUG is defined, otherwise NULL input is treated as an unhandled
 *  exception.
 */
size_t DynA_get_size (DynamicArr_t *a);

/**
 *  Changes the capacity of `a` to `new_cap`. When NDEBUG is undefined, a NULL
 *  `a` or `new_cap` value <= 0 is treated as an unhandled exception. Otherwise
 *  this returns non-zero in those cases.
 */
int DynA_resize (DynamicArr_t *a, size_t new_cap);

/**
 *  Returns the `i`-th element from `a`. When NDEBUG is undefined, a NULL `a`
 *  or an invalid index is treated as an unhandled exception. When NDEBUG is
 *  defined, this function returns NULL in those cases.
 */
void *DynA_at (DynamicArr_t *a, size_t i);

/**
 *  Appends `el` to `a`. When NDEBUG is undefined, a NULL `a` or `el` is
 *  treated as an unhandled exception. Otherwise, this function returns
 *  non-zero in those cases.
 */
int DynA_append (DynamicArr_t *a, void *el);

#endif /* _DYNA_DYNAMIC_ARRAY_LIBRARY_H_ */
