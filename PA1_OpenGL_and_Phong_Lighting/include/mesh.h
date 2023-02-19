#ifndef _MESH_H_
#define _MESH_H_
#include "defines.h"
#include <vector>

struct Vertex {
  vec3 position;
  vec3 normal;
};

class Mesh {
 public:
  /**
   * construct a mesh from a obj file
   */
  Mesh(const std::string &path);
  /**
   * draw mesh objects
   */
  void prepare();
  void draw() const;
  void deleteObject();

 private:
  //  mesh data
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

  void loadDataFromFile(const std::string &path);
};
#endif