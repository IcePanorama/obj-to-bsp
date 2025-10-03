#ifndef _WAVEFRONT_OBJ_FILE_H_
#define _WAVEFRONT_OBJ_FILE_H_

typedef struct OBJFile_s OBJFile_t;

OBJFile_t *obj_alloc (char path[static 1]);
void obj_free (OBJFile_t *o);

#endif /* _WAVEFRONT_OBJ_FILE_H_ */
