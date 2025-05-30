#include "vec3.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

Vec3_t
v3_init (char *raw_data)
{
  float elements[3] = { 0 };
  size_t el_idx = 0;

  char *tok = strtok (raw_data, " ");
  while (tok != NULL)
    {
      assert (el_idx < 3);
      elements[el_idx] = strtof (tok, NULL);
      el_idx++;
      tok = strtok (NULL, " \n");
    }

  return (Vec3_t){ .x = elements[0], .y = elements[1], .z = elements[2] };
}

#include <stdio.h>

void
v3_normalize (Vec3_t v[static 1])
{
  float m = sqrtf ((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
  if (fabsf (m - 0.0f) < 0.0001)
    return;

  v->x /= m;
  v->y /= m;
  v->z /= m;
}
