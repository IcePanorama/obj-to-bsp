#ifndef _WAVEFRONT_OBJ_FILE_H_
#define _WAVEFRONT_OBJ_FILE_H_

typedef struct WavefrontObj_s WavefrontObj_t;

/**
 *  Allocates a wavefront object file object. May return NULL on calloc
 *  failure.
 *  Return:  pointer to allocated WavefrontObj_t
 */
WavefrontObj_t *obj_alloc (void);

/**
 *  Frees all memory allocated by `obj`.
 *  Param:  obj  some WavefrontObj_t
 */
void obj_free (WavefrontObj_t *obj);

/** Param:  path  a NULL-terminated path string to some wavefront obj file. */
int obj_init (WavefrontObj_t *obj, const char path[static 1]);

#endif /* _WAVEFRONT_OBJ_FILE_H_ */
