#ifndef _WAVEFRONT_OBJ_ERRORS_H_
#define _WAVEFRONT_OBJ_ERRORS_H_

#include <stdio.h>

/**
 *  Might have multiple copies of this across the project just to keep the obj
 *  stuff self-contained.
 */
#define WAVOBJ_OOM_ERR()                                                      \
  do                                                                          \
    {                                                                         \
      fprintf (stderr, "%s: Out of memory error.\n", __func__);               \
    }                                                                         \
  while (0)

#endif /* _WAVEFRONT_OBJ_ERRORS_H_ */
