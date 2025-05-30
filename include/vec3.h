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

#endif /* _VECTOR_3_H_ */
