#include <utils.h>
#include <camera.h>
#include "enum.h"
#include <object.h>
#include <shader.h>
#include <bezier.h>
#include <bspline.h>

int index_now = 100;
int changing = 0;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float delta_time = 0.0f; // between frame and last frame
float last_frame = 0.0f; // last frame

float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees
float pitch =  0.0f;
float fov   =  45.0f;

// lighting position
glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
glm::vec3 control[] = {
glm::vec3(0.0f,0.0f, 0.0f),
    glm::vec3(0.5f,0.0f, 0.0f),
    glm::vec3( 1.0f,1.5f, 0.0f),
    glm::vec3(1.5f,0.0f, 0.0f),
    glm::vec3( 0.0f,0.0f, 0.5f),
    glm::vec3( 0.5f,0.0f, 0.5f),
    glm::vec3( 1.0f,1.0f, 0.5f),
    glm::vec3( 1.5f,0.0f, 0.5f),
    glm::vec3( 0.0f,0.0f, 1.0f),
    glm::vec3( 0.5f,0.0f, 1.0f),
    glm::vec3(1.0f,0.0f, 1.0f),
    glm::vec3( 1.5f,0.0f, 1.0f),
};

glm::vec3 control_bspline[]{
    vec3(0.0f,1.5f,-1.5f),
    vec3(0.5f,1.5f,-1.0f),
    vec3(1.0f,1.5f,-0.5f),
    vec3(1.5f,1.5f, 0.0f),
    vec3(1.0f,1.5f, 0.5f),
    vec3(0.5f,1.5f, 1.0f),
    vec3(0.0f,1.5f, 1.5f),
    vec3(0.5f,1.0f,-1.5f),
    vec3(1.0f,1.0f,-1.0f),
    vec3(1.5f,1.0f,-0.5f),
    vec3(2.0f,1.0f, 0.0f),
    vec3(1.5f,1.0f, 0.5f),
    vec3(1.0f,1.0f, 1.0f),
    vec3(0.5f,1.0f, 1.5f),
    vec3(1.0f,0.5f,-1.5f),
    vec3(1.5f,0.5f,-1.0f),
    vec3(2.0f,0.5f,-0.5f),
    vec3(2.5f,0.5f, 0.0f),
    vec3(2.0f,0.5f, 0.5f),
    vec3(1.5f,0.5f, 1.0f),
    vec3(1.0f,0.5f, 1.5f),
    vec3(1.5f,0.0f,-1.5f),
    vec3(2.0f,0.0f,-1.0f),
    vec3(2.5f,0.0f,-0.5f),
    vec3(3.0f,0.0f, 0.0f),
    vec3(2.5f,0.0f, 0.5f),
    vec3(2.0f,0.0f, 1.0f),
    vec3(1.5f,0.0f, 1.5f),
    vec3(1.0f,-0.5f,-1.5f),
    vec3(1.5f,-0.5f,-1.0f),
    vec3(2.0f,-0.5f,-0.5f),
    vec3(2.5f,-0.5f, 0.0f),
    vec3(2.0f,-0.5f, 0.5f),
    vec3(1.5f,-0.5f, 1.0f),
    vec3(1.0f,-0.5f, 1.5f),

};
glm::vec3 control_plane[] = {

    glm::vec3(0.0f,-1.0f, 0.0f),
    glm::vec3(0.5f,-1.0f, 0.0f),
    glm::vec3( 1.0f,-1.0f, 0.0f),
    glm::vec3(1.5f,-1.0f, 0.0f),
    glm::vec3( 0.0f,-1.0f, 0.5f),
    glm::vec3( 0.5f,-1.0f, 0.5f),
    glm::vec3( 1.0f,-1.0f, 0.5f),
    glm::vec3( 1.5f,-1.0f, 0.5f),
    glm::vec3( 0.0f,-1.0f, 1.0f),
    glm::vec3( 0.5f,-1.0f, 1.0f),
    glm::vec3(1.0f,-1.0f, 1.0f),
    glm::vec3( 1.5f,-1.0f, 1.0f),

};
float cubes[] = {    
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
      -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  
  };

void processInput(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double x, double y);

const int WIDTH = 800;
const int HEIGHT = 600;

bool firstMouse = true;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;

GLFWwindow *window;
Camera mycamera;

BETTER_ENUM(RenderCase, int,
            uniform_single_bezier_surface,
            uniform_multi_bezier_surface)
//            adaptive_single_bezier_surface,
//            adaptive_multi_bezier_surface,
//            single_spline_surface
/**
 * BETTER_ENUM is from https://aantron.github.io/better-enums/
 * You can add any render case as you like,
 * this is to allow you to demonstrate multi task in single executable file.
 * */


//RenderCase choice = RenderCase::_values()[0];

int main() {
    // for (size_t index = 0; index < RenderCase::_size(); ++index) {
    //     RenderCase render_case = RenderCase::_values()[index];
    //     std::cout << index << ". " << +render_case << "\n";
    // }
    // while (true) {
    //     std::cout << "choose a rendering case from [0:" << RenderCase::_size() - 1 << "]" << "\n";
    //     std::string input;
    //     std::cin >> input;
    //     if (isNumber(input) &&
    //         std::stoi(input) >= 0 &&
    //         std::stoi(input) < RenderCase::_size()) {
    //         choice = RenderCase::_values()[std::stoi(input)];
    //         break;
    //     } else {
    //         std::cout << "Wrong input.\n";
    //     }
    // }
    /**
     * Choose a rendering case in the terminal.
     * */
    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw2");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);

    Shader light_shader = Shader(getPath("shaderSource/bezier.vs"), getPath("shaderSource/bezier.fs"));
    Shader point_shader = Shader(getPath("shaderSource/points.vs"), getPath("shaderSource/points.fs"));
    // switch (choice) {
    //     case RenderCase::uniform_single_bezier_surface: {
    //         std::cout << +RenderCase::uniform_single_bezier_surface << "do something\n";
    //         break;
    //     }
    //     case RenderCase::uniform_multi_bezier_surface: {
    //         std::cout << +RenderCase::uniform_multi_bezier_surface << "do something\n";
    //         break;
    //     }
    // }

    int m = 3;
    int n = 4;
    //beziersurface
    BezierSurface surface(m, n);
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            surface.setControlPoint(i, j, control[i * n + j] + vec3(2.0, 0.0f, 0.0f));
        }
    }
    Object surface_obj = surface.generateObject();
    surface_obj.init();

    //beziersurface
    BezierSurface surface_p(m, n);
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            surface_p.setControlPoint(i, j, control_plane[i * n + j]);
        }
    }
    Object surface_p_obj = surface_p.generateObject();
    surface_p_obj.init();
    
    //bsplinesurface
    int a = 5;
    int b = 7;
    BsplineSurface bspline_surface(a, b, 3, 3);
    for(int i = 0; i < a; i++){
        for(int j = 0; j < b; j++){
            bspline_surface.setControlPoint(i, j, control_bspline[i * b + j] - vec3(5.0, 0.0f, 0.0f));
        }
    }
    Object bsplineObject = bspline_surface.generateObject();
    bsplineObject.init();
    
    // multiple bezier surfaces
    std::vector<BezierSurface> surfaces;
    std::vector<Object> objects; 
    surfaces = read(getPath("assets/tea.bzs"));
    // for(int i = 0; i < 16; i++){
    //     std::cout << surfaces[0].inds[i];
    // }
    for(int i = 0; i < surfaces.size(); i++){
        Object surface_obj = surfaces[i].generateObject();
        surface_obj.init();
        objects.push_back(surface_obj);
    }

    GLuint p_VAO;
    GLuint p_VBO;
    glGenVertexArrays(1, &p_VAO);
    glBindVertexArray(p_VAO);
    glGenBuffers(1, &p_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, p_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubes), cubes, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        if(changing){   
            if(index_now < 12){
                surface_p.setControlPoint(int(index_now/n), index_now % n, control_plane[index_now]);
                surface_p_obj = surface_p.generateObject();
                surface_p_obj.init();
            }           
            changing = 0;
        }
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        light_shader.use();
        // light_shader.setVec3("object_color", vec3(1.0f, 0.5f, 0.31f));
        light_shader.setVec3("object_color", vec3(1.0f, 1.0f, 1.0f));
        light_shader.setVec3("light_color", vec3(1.0f, 1.0f, 1.0f));
        light_shader.setVec3("light_pos", lightPos);
        light_shader.setVec3("view_pos", cameraPos);
        // light_shader.setVec3("cone_direction", mycamera.Front);
        // light_shader.setFloat("cone_angle", 10.0f);
        light_shader.setFloat("light_attenuation", 0.032f);
        // light_shader.setFloat("cutoff", glm::cos(glm::radians(10.5f)));
        // light_shader.setFloat("outer_cutoff", glm::cos(glm::radians(15.5f)));
        
        
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::mat4(1.0f);
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        light_shader.setMat4("projection", projection);
        light_shader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        light_shader.setMat4("model", model);

        float current_time = glfwGetTime();
		delta_time = current_time - last_frame;
		last_frame = current_time;

        surface_obj.drawElements();
        surface_p_obj.drawElements();
        for(int i = 0; i < objects.size(); i++){
            objects[i].drawElements();
        }
        // bsplineObject.draw_mode.primitive_mode = GL_TRIANGLES;
        bsplineObject.drawElements();

        point_shader.use();
        point_shader.setMat4("projection", projection);
        point_shader.setMat4("view", view);

        glBindVertexArray(p_VAO);
        for(int i = 0; i < 3 * 4; i++){
            model = glm::mat4(1.0f);
            model = glm::translate(model, control_plane[i]);
            model = glm::scale(model, glm::vec3(0.05f)); 
            point_shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}

// void processInput(GLFWwindow *window) {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
//     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//         mycamera.processWalkAround(Forward);
//     }
//     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//         mycamera.processWalkAround(Backward);
//     }
//     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//         mycamera.processWalkAround(Leftward);
//     }
//     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
//         mycamera.processWalkAround(Rightward);
//     }
//     if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
//         std::cout << "Camera position: (" << mycamera.Position.x << ", "
//                   << mycamera.Position.y << ", " << mycamera.Position.z << ")" << std::endl;
//     }
// }
void processInput(GLFWwindow* window){
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

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        int screen_w,screen_h;
        int pixel_w,pixel_h;
        double xpos,ypos,zpos;
        glfwGetWindowSize(window, &screen_w, &screen_h);
        glfwGetFramebufferSize(window, &pixel_w, &pixel_h);
        glfwGetCursorPos(window, &xpos, &ypos);
        glm::vec2 screen_pos=glm::vec2(xpos, ypos);
        glm::vec2 pixel_pos=screen_pos * glm::vec2(pixel_w, pixel_h) / glm::vec2(screen_w, screen_h);
        pixel_pos = pixel_pos + glm::vec2(0.5f, 0.5f);
        glm::vec3 win=glm::vec3(pixel_pos.x, pixel_h-pixel_pos.y, 0.0f);
        glReadPixels( (GLint)win.x, (GLint)win.y,1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &win.z);

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::vec4 viewport(0.0f,0.0f,(float)WIDTH, (float)HEIGHT);
        glm::vec3 world = glm::unProject(win, view, projection, viewport);

        // std::cout <<"screen " << xpos <<" " << ypos <<" " << std::endl;
        // std::cout <<"world " << world.x <<" " << world.y <<" " << world.z << std::endl;

        // find index 
        int index = 100;
        float min_dis = 10.0f; 
        for(int i = 0; i < 12; i++){
            float tmp = glm::length(world - glm::vec3(control_plane[i]));
            if(min_dis > tmp){
                min_dis = tmp;
                index = i;
            }
        }
        if(index < 12)
            index_now = index;
            if(glm::length(world - control_plane[index_now]) < 1)
                control_plane[index_now] = world;
        changing = 1;
    }		
}

void mouse_callback(GLFWwindow* window, double x_pos_in, double y_pos_in){
    float x_pos = static_cast<float>(x_pos_in);

    float y_pos = static_cast<float>(y_pos_in);

    if(firstMouse){
      lastX = x_pos;
      lastY = y_pos;
      firstMouse = false;
    }

    float x_offset = x_pos - lastX;
    float y_offset = lastY - y_pos; // reversed because y-coordinates go from bottom to top
    
    lastY = y_pos;
    lastX = x_pos;
    

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