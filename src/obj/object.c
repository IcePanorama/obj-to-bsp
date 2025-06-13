#include "obj/object.h"
#include "obj/face.h"
#include "obj/normal.h"
#include "obj/vert_coords.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct NamedObject_s
{
  char *name;
  DynamicArray_t *faces;
  bool smooth_shading;
};

NamedObject_t *
no_alloc (void)
{
  NamedObject_t *o = calloc (1, sizeof (NamedObject_t));
  if (o == NULL)
    return NULL;

  o->faces = dyna_alloc (sizeof (Face_t));
  if (o->faces == NULL)
    {
      free (o);
      return NULL;
    }

  return o;
}

void
no_free (NamedObject_t *no)
{
  if (no == NULL)
    return;

  if (no->name != NULL)
    free (no->name);

  if (no->faces != NULL)
    dyna_free (no->faces);

  free (no);
}

/**
 *  Peeks to check whether the next line in `fptr` begins with an 'o'. Also
 *  returns true if EOF is reached.
 */
static bool
peek_eos (FILE *fptr)
{
  char next = getc (fptr);
  ungetc (next, fptr);
  return (next == EOF) || (next == 'o');
}

static int
init_name (NamedObject_t *no, const char *name)
{
  no->name = malloc (sizeof (char) * (strlen (name) + 1));
  if (no->name == NULL)
    return -1;
  strcpy (no->name, name);
  no->name[strlen (no->name) - 1] = '\0'; // replace '\n' w/ '\0'
  return 0;
}

int
no_init (NamedObject_t *no, const char name[static 1],
         FILE input_fptr[static 1], DynamicArray_t *verts,
         DynamicArray_t *norms)
{
  if (no == NULL)
    return -1;

  if (init_name (no, name) != 0)
    return -1;

  while (!peek_eos (input_fptr))
    {
      char curr_line[256] = { 0 };
      // will never return NULL because of `peek_eos`
      fgets (curr_line, 256, input_fptr);

      if (strncmp (curr_line, "vn", 2) == 0)
        {
          Normal_t n = nm_init (curr_line + 3);
          if (dyna_append (norms, &n) != 0)
            goto append_failure;
        }
      else if (strncmp (curr_line, "vt", 2) == 0)
        {
          printf ("%s", curr_line + 3);
        }
      else if (curr_line[0] == 'f')
        {
          Face_t f = fc_init (curr_line + 2, verts, norms);
          if (dyna_append (no->faces, &f) != 0)
            goto append_failure;
        }
      else if (curr_line[0] == 's')
        {
          no->smooth_shading = (curr_line[2] == '1');
        }
      else if (curr_line[0] == 'v')
        {
          VertCoord_t v = vc_init (curr_line + 2);
          if (dyna_append (verts, (void *)&v) != 0)
            goto append_failure;
        }
      else
        {
          fprintf (stderr, "%s: Unrecognized line %s", __func__, curr_line);
          return -1;
        }
    }

  return 0;
append_failure:
  fprintf (stderr, "%s: append failed.\n", __func__);
  return -1;
}

void
no_print (NamedObject_t *no)
{
  printf ("Name: %s\n", no->name);
  printf ("Smooth shading? %s\n", no->smooth_shading ? "Yes" : "No");
  puts ("Faces: ");
  dyna_print (no->faces, fc_print);
}
