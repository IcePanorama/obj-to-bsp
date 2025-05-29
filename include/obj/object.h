#ifndef _WAVEFRONT_OBJ_OBJECT_H_
#define _WAVEFRONT_OBJ_OBJECT_H_

typedef struct NamedObject_s NamedObject_t;

NamedObject_t *no_alloc (void);
void no_free (NamedObject_t *no);
int no_init (NamedObject_t *no, const char line[static 1]);
void no_print (NamedObject_t *no);

#endif /* _WAVEFRONT_OBJ_OBJECT_H_ */
