#include "obj/normal.h"
#include "vec3.h"

Normal_t
nm_init (char *raw_data)
{
  Normal_t n = v3_init (raw_data);
  v3_normalize (&n);
  return n;
}
