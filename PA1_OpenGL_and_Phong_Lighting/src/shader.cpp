#include <shader.h>
#include <utils.h>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
  init(vsPath, fsPath, gsPath);
}

void Shader::init(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
  if(gsPath != ""){
    initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath), getCodeFromFile(gsPath));
  }else{
    initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath), "");
  }
}

void Shader::initWithCode(const std::string &vs, const std::string &fs, const std::string &gs) {
// TODO: filling this function to set shaders to OpenGL
  // 2. compile shaders
  // In order for OpenGL to use the shader it has to dynamically compile it at run-time from its source code.
  // create a shader object, referenced by an ID
  GLuint vShader;
  vShader = glCreateShader(GL_VERTEX_SHADER);
  const char* vShaderCode = vs.c_str();
  // attach the shader source code to the shader object and compile
  // The second argument specifies how many strings we're passing as source code. The third parameter is the actual source code of the vertex shader 
  glShaderSource(vShader, 1, &vShaderCode, NULL);
  glCompileShader(vShader);

  // check for compile-time error
  int  success;
  char infoLog[512];
  glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(vShader, 512, NULL, infoLog);
    std::cout << "VERTEXSHADER_COMPILATION_FAILED" << std::endl;
    std::cout << infoLog << std::endl;
  }

  // The fragment shader is all about calculating the color output of your pixels
  // The process for compiling a fragment shader is similar to the vertex shader
  GLuint fShader;
  fShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char* fShaderCode = fs.c_str();
  glShaderSource(fShader, 1, &fShaderCode, NULL);
  glCompileShader(fShader);

  // check for compile-time error
  glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(fShader, 512, NULL, infoLog);
    std::cout << "FRAGMENTSHADER_COMPILATION_FAILED" << std::endl;
    std::cout << infoLog << std::endl;
  }


  GLuint gShader;
  if(gs != ""){
    
    gShader = glCreateShader(GL_GEOMETRY_SHADER);
    const char* gShaderCode = gs.c_str();
    glShaderSource(gShader, 1, &gShaderCode, NULL);
    glCompileShader(gShader);

    // check for compile-time error
    glGetShaderiv(gShader, GL_COMPILE_STATUS, &success);
    if(!success){
      glGetShaderInfoLog(gShader, 512, NULL, infoLog);
      std::cout << "GEOMETRYSHADER_COMPILATION_FAILED" << std::endl;
      std::cout << infoLog << std::endl;
    }
  }

  // The glCreateProgram function creates a program and returns the ID reference to the newly created program object.
  // GLuint shaderProgram;
  id = glCreateProgram();
  glAttachShader(id, vShader);
  glAttachShader(id, fShader);
  if(gs != ""){
    glAttachShader(id, gShader);
  }
  glLinkProgram(id);

  // check for link error
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if(!success){
    glGetProgramInfoLog(id, 512, NULL, infoLog);
    std::cout << "SHADERPROGRAM_LINKING_FAILED" << std::endl;
    std::cout << infoLog << std::endl;
  }
  //emm... it seems that LOG_ERR LOG_INFO has been defined in defines.h

  // delete the shaders as they're linked into our program now
  glDeleteShader(vShader);
  glDeleteShader(fShader);
  if(gs != ""){
    glDeleteShader(gShader);
  }
}

std::string Shader::getCodeFromFile(const std::string &path) {
  // 1. retrieve the vertex/fragment source code from filePath
  std::string code;
  std::ifstream file;
  // ensure ifstream objects can throw exceptions:
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(getPath(path));
    std::stringstream stream;
    // read file's buffer contents into streams
    stream << file.rdbuf();
    // close file handlers
    file.close();
    // convert stream into string
    code = stream.str();
  } catch (std::ifstream::failure &e) {
    LOG_ERR("File Error: " + std::string(e.what()));
  }
  return code;
}

void Shader::use() const { glUseProgram(id); }

GLint Shader::getUniformLocation(const std::string &name) const {
  return glGetUniformLocation(id, name.c_str());
}

void Shader::setInt(const std::string &name, GLint value) const {
  glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, GLfloat value) const {
  glUniform1f(getUniformLocation(name), value);
}

void Shader::setMat3(const std::string &name, const mat3 &value) const {
  glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const mat4 &value) const {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, const vec3 &value) const {
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const vec4 &value) const {
  glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
