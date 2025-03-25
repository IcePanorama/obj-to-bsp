#include "dynamic_arr.h"
#include "log.h"

#include <string.h>

struct _DynamicArray_s
{
  size_t size;     // logical size
  size_t capacity; // total possible size
  size_t el_size;  // size in bytes of one element of the array
  void *data;
};

DynamicArray_t *
dyna_create (size_t el_size)
{
  DynamicArray_t *out = calloc (1, sizeof (DynamicArray_t));
  if (out == NULL)
    return NULL;

  out->size = 0;
  out->capacity = 1;
  out->el_size = el_size;
  out->data = calloc (out->capacity, out->el_size);
  return out;
}

void
dyna_free (DynamicArray_t *arr)
{
  if (arr == NULL)
    return;

  if (arr->data != NULL)
    free (arr->data);
  free (arr);
}

static int
double_size (DynamicArray_t *arr)
{
  return dyna_resize (arr, arr->capacity * 2);
}

int
dyna_append (DynamicArray_t *arr, void *el)
{
  if ((el == NULL) || (arr == NULL))
    {
      return -1;
    }

  if (arr->size == arr->capacity)
    {
      if (double_size (arr) != 0)
        {
          LOG_ERROR ("Realloc failure for dynamic array of size, %ld.",
                     arr->capacity);
        }
      return -1;
    }

  void *ptr = (void *)((uint8_t *)(arr->data) + (arr->el_size * arr->size));
  memcpy (ptr, el, arr->el_size);
  arr->size++;
  return 0;
}

int
dyna_resize (DynamicArray_t *arr, size_t new_capacity)
{
  if ((arr == NULL) || (new_capacity < arr->capacity))
    return -1;

  arr->capacity = new_capacity;
  void *tmp = realloc (arr->data, arr->capacity * arr->el_size);
  if (tmp == NULL)
    {
      return -1;
    }

  arr->data = tmp;
  return 0;
}
