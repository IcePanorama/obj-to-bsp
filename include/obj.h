#ifndef _WAVEFRONT_OBJ_FILE_H_
#define _WAVEFRONT_OBJ_FILE_H_

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>

typedef struct ObjFile_s
{
  struct VertexCoord_s
  {
    float x;
    float y;
    float z;
    float w; // optional, defaults to 1.0.
  } *verticies_list;
  size_t num_verticies;
  size_t max_num_verticies;

  struct TextureCoord_s
  {
    float u;
    float v; // optional, defaults to 0.
    float w; // optional, defaults to 0.
  } *texture_coords_list;
  size_t num_texture_coords;
  size_t max_num_texture_coords;

  /** Might not be unit vectors. */
  struct VertexNormal_s
  {
    float x;
    float y;
    float z;
  } *vertex_normals_list;
  size_t num_vertex_normals;
  size_t max_num_vertex_normals;

  /** Free form geometry statement. */
  struct ParameterSpaceVertex_s
  {
    float u;
    float v; // optional, defaults to 0.
    float w; // optional, defaults to 0.
  } *parameter_space_verticies_list;
  size_t num_parameter_space_verticies;
  size_t max_num_parameter_space_verticies;

  /**
   *  Stores index of its verticies, texture coordinates, & vertex normals.
   *  Only supporting triangles for easier centroid calculations.
   */
  struct PolygonalFace_s
  {
    ssize_t vertices[3];
    ssize_t texture_coords[3]; // optional
    ssize_t vertex_normals[3]; // optional
  } *faces_list;
  size_t num_faces;
  size_t max_num_faces;

  // Won't be using these or Objects for bsp trees, I think?
  struct Polyline_s
  {
    struct VertexCoord_s **vertices;
    size_t max_num_poly_line_verticies;
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
