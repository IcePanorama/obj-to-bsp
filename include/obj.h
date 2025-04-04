#ifndef _WAVEFRONT_OBJ_FILE_H_
#define _WAVEFRONT_OBJ_FILE_H_

#include "dynamic_arr.h"

// FIXME: move to own file!
struct VertexCoord_s
{
  float x;
  float y;
  float z;
  float w; // optional, defaults to 1.0.
};

// FIXME: move to own file?
struct TextureCoord_s
{
  float u;
  float v; // optional, defaults to 0.
  float w; // optional, defaults to 0.
};

// FIXME: move to own file?
/** May not be a unit vector. */
struct VertexNormal_s
{
  float x;
  float y;
  float z;
};

// FIXME: move to own file?
/** Free form geometry statement. */
struct ParameterSpaceVertex_s
{
  float u;
  float v; // optional, defaults to 0.
  float w; // optional, defaults to 0.
};

struct PolygonalFace_s
{
  struct VertexCoord_s *verts[3];
  struct VertexNormal_s *norms[3];      // optional
  struct TextureCoord_s *tex_coords[3]; // optional
};

typedef struct ObjFile_s
{
  DynamicArray_t *vertices_list;
  DynamicArray_t *texture_coords_list;
  DynamicArray_t *vertex_normals_list;
  DynamicArray_t *parameter_space_vertices_list;
  DynamicArray_t *faces_list;

  // Won't be using these or Objects for bsp trees, I think?
  struct Polyline_s
  {
    struct VertexCoord_s **vertices;
    size_t max_num_poly_line_vertices;
  } *poly_lines_list;
  size_t num_poly_lines;
  size_t max_num_poly_lines;

  struct Object_s
  {
    char *name;
    size_t obj_name_length;
    struct PolygonalFace_s **faces_list;
    size_t max_num_obj_faces;
  } *objects_list;
  size_t num_objs;
  size_t max_num_objs;
} ObjFile_t;

int create_obj_file_from_file (ObjFile_t o[static 1],
                               const char file_path[static 1]);
void free_obj_file (ObjFile_t *o);

#endif /* _WAVEFRONT_OBJ_FILE_H_ */
