#include <object.h>
#include <shader.h>
#include <utils.h>
#include <fstream>
#include <vector>

Object::Object() {}
Object::~Object() {}

/**
 * TODO: initialize VAO, VBO, VEO and set the related varialbes
 */
void Object::init() {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  // UNIMPLEMENTED;
  // Generate vertex array object, vertex buffer object, element/index buffer object 
  glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
  // Fisrtly, bind vertex array oject
  glBindVertexArray(VAO);

  // Then bind vertex buffer object(transfer data to GPU's memory and manage the memory) to GL_ARRAY_BUFFER(target)
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // After glBindBuffer, on any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the currently bound buffer, which is VBO. 
  // Then, copy vertex data to buffer's memory
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  // EBO is similar
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glEnableVertexAttribArray(0);
  // normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  // the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * TODO: draw object with VAO and VBO
 * You can choose to implement either one or both of the following functions.
 */

/* Implement this one if you do not use a shader */
void Object::drawArrays() const {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  // UNIMPLEMENTED;
  glBindVertexArray(VAO);
	glDrawArrays(draw_mode.primitive_mode, 0, vertices.size());
	glBindVertexArray(0);
}

/* Implement this one if you do use a shader */
void Object::drawArrays(const Shader& shader) const {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  UNIMPLEMENTED;
}

/**
 * TODO: draw object with VAO, VBO, and VEO
 * You can choose to implement either one or both of the following functions.
 */

/* Implement this one if you do not use a shader */
void Object::drawElements() const {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  // UNIMPLEMENTED;
  glBindVertexArray(VAO);
	glDrawElements(draw_mode.primitive_mode, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

/* Implement this one if you do use a shader */
void Object::drawElements(const Shader& shader) const {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  UNIMPLEMENTED;
}