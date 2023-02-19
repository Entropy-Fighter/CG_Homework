#include <utils.h>
#include <mesh.h>
#include <shader.h>

const int WIDTH = 1920;
const int HEIGHT = 1080;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float delta_time = 0.0f; // between frame and last frame
float last_frame = 0.0f; // last frame

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees
float pitch =  0.0f;
float last_x =  1920.0f / 2.0;
float last_y =  1080.0 / 2.0;
float fov   =  45.0f;

// lighting position
glm::vec3 lightPos(6.2f, 1.0f, 2.0f);

void process_input(GLFWwindow* window);

GLFWwindow *window;
//initialize
int main() {
  WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw1");
  float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
  /*
  //test--load mesh info
  Mesh mesh(getPath("assets/plane.obj"));
  std::cout << "vertex information" << std::endl;
  for(int i = 0; i < mesh.vertices.size(); i++){
    std::cout << mesh.vertices[i].position[0] << " ";
    std::cout << mesh.vertices[i].position[1] << " ";
    std::cout << mesh.vertices[i].position[2] << std::endl;
  }
  std::cout << "normal information" << std::endl;
  for(int i = 0; i < mesh.vertices.size(); i++){
    std::cout << mesh.vertices[i].normal[0] << " ";
    std::cout << mesh.vertices[i].normal[1] << " ";
    std::cout << mesh.vertices[i].normal[2] << std::endl;
  }
  std::cout << "Indices information" << std::endl;
  for(int i = 0; i < mesh.indices.size(); i++){
    std::cout << mesh.indices[i] << std::endl;
  }
  */

  

  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);


  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  

  Shader light_shader = Shader(getPath("shaderSourceCode/light.vs"), getPath("shaderSourceCode/light.fs"), "");
  Shader shader = Shader(getPath("shaderSourceCode/vsSourceCode.vs"), getPath("shaderSourceCode/fsSourceCode.fs"), "");
  Shader fur_shader = Shader(getPath("shaderSourceCode/fur.vs"), getPath("shaderSourceCode/fur.fs"), getPath("shaderSourceCode/fur.gs"));
 
  
  Mesh mesh = Mesh(getPath("assets/bunny.obj"));
  mesh.prepare();
  

  glEnable(GL_DEPTH_TEST);
  while (!glfwWindowShouldClose(window)) {
    process_input(window);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

  
    light_shader.use();
    // light_shader.setVec3("object_color", vec3(1.0f, 0.5f, 0.31f));
    light_shader.setVec3("object_color", vec3(1.0f, 1.0f, 1.0f));
    light_shader.setVec3("light_color", vec3(1.0f, 1.0f, 1.0f));
    light_shader.setVec3("light_pos", cameraPos);
    light_shader.setVec3("view_pos", cameraPos);
    light_shader.setVec3("cone_direction", cameraFront);
    light_shader.setFloat("cone_angle", 10.0f);
    light_shader.setFloat("light_attenuation", 0.032f);
    light_shader.setFloat("cutoff", glm::cos(glm::radians(10.5f)));
    light_shader.setFloat("outer_cutoff", glm::cos(glm::radians(15.5f)));
    
    
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(1.0f);
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    light_shader.setMat4("projection", projection);
    light_shader.setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    light_shader.setMat4("model", model);
 

    // //activate shader
    // shader.use();
    // // model matrix
    // //glm::mat4 model = glm::mat4(1.0f);
    // shader.setMat4("model", model);
    // // view matrix
    // //glm::mat4 view = glm::mat4(1.0f);
    // //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    // view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    float current_time = glfwGetTime();
		delta_time = current_time - last_frame;
		last_frame = current_time;
    // shader.setMat4("view", view);
    // // projection matrix fov near far
    // //glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(WIDTH) / float(HEIGHT), 0.1f, 100.0f);
    // shader.setMat4("projection", projection);
    
    
    
    mesh.draw();

    fur_shader.use();
    fur_shader.setMat4("projection", projection);
    fur_shader.setMat4("view", view);
    fur_shader.setMat4("model", model);
    // fur_shader.setFloat("time", static_cast<float>(glfwGetTime()));
    fur_shader.setVec3("object_color", vec3(1.0f, 1.0f, 1.0f));
    fur_shader.setVec3("light_color", vec3(1.0f, 1.0f, 1.0f));
    fur_shader.setVec3("light_pos", cameraPos);
    fur_shader.setVec3("view_pos", cameraPos);
    fur_shader.setVec3("cone_direction", cameraFront);
    fur_shader.setFloat("cone_angle", 10.0f);
    fur_shader.setFloat("light_attenuation", 0.032f);
    fur_shader.setFloat("cutoff", glm::cos(glm::radians(10.5f)));
    fur_shader.setFloat("outer_cutoff", glm::cos(glm::radians(15.5f)));
    mesh.draw();
    
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  return 0;
}

void process_input(GLFWwindow* window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}

	float camera_speed = 2.5f * delta_time; // adjust accordingly 0.05f
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
    cameraPos += camera_speed * cameraFront;
  }else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
    cameraPos -= cameraFront * camera_speed;
  }else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
    cameraPos -= camera_speed * glm::normalize(glm::cross(cameraFront, cameraUp));
  }else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
    cameraPos += camera_speed * glm::normalize(glm::cross(cameraFront, cameraUp));
  }
		
}





void mouse_callback(GLFWwindow* window, double x_pos_in, double y_pos_in){
    float x_pos = static_cast<float>(x_pos_in);

    float y_pos = static_cast<float>(y_pos_in);

    if(firstMouse){
      last_x = x_pos;
      last_y = y_pos;
      firstMouse = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos; // reversed because y-coordinates go from bottom to top
    
    last_y = y_pos;
    last_x = x_pos;
    

    float sensitivity = 0.1f; 
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    pitch += y_offset;

    yaw += x_offset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if(pitch > 89.0f){
      pitch = 89.0f;
    }
        
    if(pitch < -89.0f){
      pitch = -89.0f;
    }
        

    glm::vec3 front;

    front[0] = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front[1] = sin(glm::radians(pitch));
    front[2] = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    fov -= (float)y_offset;
    if(fov < 1.0f){
      fov = 1.0f;
    }
        
    if(fov > 45.0f){
      fov = 45.0f;
    }
        
}