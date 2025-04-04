#ifndef _WAVEFRONT_OBJ_POLYGONAL_FACE_H_
#define _WAVEFRONT_OBJ_POLYGONAL_FACE_H_

struct PolygonalFace_s
{
  struct OBJVertexCoord_s *verts[3];
  struct VertexNormal_s *norms[3];      // optional
  struct TextureCoord_s *tex_coords[3]; // optional
};

#endif /* _WAVEFRONT_OBJ_POLYGONAL_FACE_H_ */
