#ifndef _WAVEFRONT_OBJ_POLYGONAL_FACE_H_
#define _WAVEFRONT_OBJ_POLYGONAL_FACE_H_

/**
 *  "Vertices are stored in counter-clockwise order by default."
 *  See: https://en.wikipedia.org/wiki/Wavefront_.obj_file
 */
struct PolygonalFace_s
{
  struct OBJVertexCoord_s *verts[3];
  struct VertexNormal_s *norms[3];      // optional
  struct TextureCoord_s *tex_coords[3]; // optional
};

#endif /* _WAVEFRONT_OBJ_POLYGONAL_FACE_H_ */
