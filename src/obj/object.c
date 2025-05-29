#include "obj/object.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct NamedObject_s
{
  char *name;
// VertexCoord_t *verts;
// Face_t *faces;
};

NamedObject_t *
no_alloc (void)
{
  return calloc (1, sizeof (NamedObject_t));
}

void
no_free (NamedObject_t *no)
{
  if (no == NULL)
    return;

  if (no->name != NULL)
    free (no->name);
  free (no);
}

int
no_init (NamedObject_t *no, const char line[static 1])
{
  if (no == NULL)
    return -1;

  no->name = malloc (sizeof (char) * (strlen (line) + 1));
  if (no->name == NULL)
    return -1;
  strcpy (no->name, line);
  no->name[strlen (no->name) - 1] = '\0'; // replace '\n' w/ '\0'

  return 0;
}

void
no_print (NamedObject_t *no)
{
  printf ("Name: %s\n", no->name);
}
