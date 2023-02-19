#include <mesh.h>
#include <utils.h>
#include <fstream>
#include <vector>

#include <sstream>
#include <string>


Mesh::Mesh(const std::string &path) { loadDataFromFile(getPath(path)); }

/**
 * TODO: implement load mesh data from file here
 * you need to open file with [path]
 *
 * File format
 *
 *
 * For each line starting with 'v' contains 3 floats, which
 * represents the position of a vertex
 *
 * For each line starting with 'n' contains 3 floats, which
 * represents the normal of a vertex
 *
 * For each line starting with 'f' contains 6 integers
 * [v0, n0, v1, n1, v2, n2], which represents the triangle face
 * v0, n0 means the vertex index and normal index of the first vertex
 * v1, n1 means the vertex index and normal index of the second vertex
 * v2, n2 means the vertex index and normal index of the third vertex
 */
void Mesh::loadDataFromFile(const std::string &path) {
    /**
     * path stands for the where the object is storaged
     * */
    // std::ifstream objectFile;
    // objectFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    // std::string objectString;
    // try{
    //   objectFile.open(path);
    //   std::stringstream objectStream;
    //   objectStream << objectFile.rdbuf();
    //   objectFile.close();
    //   objectString = objectStream.str();
    // }
    // catch(std::ifstream::failure e){
    //     std::cout << "OBJECTFILE__READ_FAILURE" << std::endl;
    // }
    std::ifstream objectFile;
    std::string eachLine;
    objectFile.open(path);
    if(!objectFile.is_open()){
      std::cout << "OBJECTFILE__READ_FAILURE" << std::endl;
      return;
    }

    std::vector<vec3> normals;
    while(std::getline(objectFile, eachLine)){
      // int pos = eachLine.find_first_of("#");
      // if(pos != std::string::npos){
      //   continue;
      // }
      if(eachLine == ""){
        continue;
      }
      std::stringstream lineStream(eachLine);

      std::vector<float> normal; 
      float normalInfo;

      std::string type;
      lineStream >> type;
      if(type == "#"){
        continue;
      }else if(type == "v"){
        Vertex tmp;
        lineStream >> tmp.position[0] >> tmp.position[1] >> tmp.position[2];
        vertices.push_back(tmp);
      }else if(type == "n"){
        while(lineStream >> normalInfo){
          normal.push_back(normalInfo);
        }
        normals.push_back(vec3(normal[0], normal[1], normal[2]));        
      }else if(type == "f"){
        std::vector<int> face;
        int faceInfo;
        while(lineStream >> faceInfo){
          face.push_back(faceInfo);
        }
        indices.push_back(face[0]);
        vertices[face[0]].normal = normals[face[1]];
        indices.push_back(face[2]);
        vertices[face[2]].normal = normals[face[3]];
        indices.push_back(face[4]);
        vertices[face[4]].normal = normals[face[5]];
      }

    }
    

    

    

  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  //std::ifstream infile(path);

  //UNIMPLEMENTED;
}

/**
 * TODO: implement your draw code here
 */
void Mesh::prepare(){
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

  // Configure vertex attributes
  // position 
  // note: 0 is the layout position of this attrib, sizeof(Vertex) is the stepsize, (void*)0 is the offset
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glEnableVertexAttribArray(0);
  // normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  // the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // DO NOT unbind the EBO while a VAO is active as the bound element buffer object is stored in the VAO.
  // A VAO stores the glBindBuffer calls when the target is GL_ELEMENT_ARRAY_BUFFER. 
  // This also means it stores its unbind calls so make sure you don't unbind the element array buffer before unbinding your VAO, otherwise it doesn't have an EBO configured.

  // glBindVertexArray(0);
  // note: You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
}

void Mesh::draw() const {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  // UNIMPLEMENTED;
  glBindVertexArray(VAO); // we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	// glBindVertexArray(0);
}

void Mesh::deleteObject(){
  // optional: de-allocate all resources once they've outlived their purpose:
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}