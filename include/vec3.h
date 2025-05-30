#ifndef _VECTOR_3_H_
#define _VECTOR_3_H_

typedef struct Vec3_s
{
  float x;
  float y;
  float z;
} Vec3_t;

Vec3_t v3_init (char *raw_data);

void v3_normalize (Vec3_t v[static 1]);

/** Param is of type `void *`, so we can use this with `dyna_print`. */
void v3_print (void *v);

#endif /* _VECTOR_3_H_ */
