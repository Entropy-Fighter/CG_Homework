#include "cloth.h"
#include "scene.h"


Vec3 old_world = Vec3(0.0f, 0.0f, 0.0f);
int changing = 0;
Vec3 old_p;
float similar_ratio = 0;
int i = 100;

int main() {

  /// settings

  // window
  constexpr int window_width = 1920;
  constexpr int window_height = 1080;

  // cloth
  constexpr Float cloth_weight = Float(2);
  constexpr UVec2 mass_dim = { 40, 30 };
  constexpr Float dx_local = Float(0.1);
  constexpr Float stiffness = Float(15);
  constexpr Float damping_ratio = Float(0.0015);
  std::vector<IVec2> fixed_masses { { 0, -1 }, { -1, -1 } };



  /// setup window
  GLFWwindow* window;
  {
    if (!glfwInit()) // initialize glfw library
      return -1;

    // setting glfw window hints and global configurations
    {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // use core mode
      // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // use debug context
    #ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    #endif
    }

    // create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(window_width, window_height, "CS171 HW5: Cloth Simulation", NULL, NULL);
    if (!window) {
      glfwTerminate();
      return -1;
    }

    // make the window's context current
    glfwMakeContextCurrent(window);

    // load Opengl
    if (!gladLoadGL()) {
      glfwTerminate();
      return -1;
    }

    // setup call back functions
    glfwSetFramebufferSizeCallback(window, Input::CallBackResizeFlareBuffer);
  }

  /// main Loop
  {
    // shader
    Shader::Initialize();

    // scene
    Scene scene(45);
    scene.camera.transform.position = { 0, -1.5, -6 };
    scene.camera.transform.rotation = { 0, 0, 1, 0 };
    scene.light_position = { 0, 3, -10 };
    scene.light_color = Vec3(1, 1, 1) * Float(1.125);

    // clothes
    auto cloth = std::make_shared<RectCloth>(cloth_weight,
                                             mass_dim,
                                             dx_local,
                                             stiffness, damping_ratio);
    for (const auto& fixed_mass : fixed_masses) {
      if (!cloth->SetMassFixedOrNot(fixed_mass.x, fixed_mass.y, true))
        abort();
    }

    // mesh primitives
    auto mesh_cube = std::make_shared<Mesh>(MeshPrimitiveType::cube);
    auto mesh_sphere = std::make_shared<Mesh>(MeshPrimitiveType::sphere);

    // objects
    auto object_cloth = scene.AddObject(cloth,
                                        Shader::shader_phong,
                                        Transform(Vec3(0, 0, 0),
                                                  glm::quat_cast(glm::rotate(Mat4(Float(1.0)), glm::radians(Float(60)), Vec3(1, 0, 0))),
                                                  Vec3(1, 1, 1)));
    auto object_cube = scene.AddObject(mesh_cube,
                                       Shader::shader_phong,
                                       Transform(Vec3(-3.5, -1.8, 0.3),
                                                 Quat(1, 0, 0, 0),
                                                 Vec3(1, 1, 1)));
    // auto object_sphere = scene.AddObject(mesh_sphere,
    //                                      Shader::shader_phong,
    //                                      Transform(Vec3(3.5, -1.8, 0.3),
    //                                                Quat(1, 0, 0, 0),
    //                                                Vec3(1, 1, 1)));
    auto object_sphere = scene.AddObject(mesh_sphere,
                                         Shader::shader_phong,
                                         Transform(Vec3(0, -1.8, 0.3),
                                                   Quat(1, 0, 0, 0),
                                                   Vec3(2.0, 2.0, 2.0)));
    object_cloth->color = { zero, Float(0.75), one };
    object_cube->color = { Float(0.75), one, zero };
    object_sphere->color = { one, Float(0.75), zero };

    // loop until the user closes the window
    Input::Start(window);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
      Input::Update();
      Time::Update();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      /// terminate
      if (Input::GetKey(KeyCode::Escape))
        glfwSetWindowShouldClose(window, true);

      // int index = 100;
      // if(1){
      if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
        Vec3 ray = (Input::mouse_position.x / (Input::window_width * 0.5f) - 1) * scene.camera.near * tanf(scene.camera.fov_y * 0.5f * pi / 180) * scene.camera.aspect * scene.camera.transform.Right() + 
        (Input::mouse_position.y / (Input::window_height * 0.5f) - 1) * scene.camera.near * tanf(scene.camera.fov_y * 0.5f * pi / 180) * scene.camera.transform.Up() + 
        scene.camera.near * scene.camera.transform.Forward() ;
        Vec3 new_p = ray + scene.camera.transform.position;
        
        if(i == 100){
          cloth->LocalToWorldPositions();
          float d = 99999;
          for(int ih = 0; ih < mass_dim.y; ih++){
            for(int iw = 0; iw < mass_dim.x; iw++){
              Vec3 cloth_camera = cloth->local_or_world_positions[cloth->Get1DIndex(iw, ih)] - scene.camera.transform.position;
              if(glm::length(cloth_camera - glm::dot(cloth_camera, glm::normalize(ray)) * glm::normalize(ray)) <= 0.06){
                if(d > glm::dot(cloth_camera, glm::normalize(ray))){
                  i = cloth->Get1DIndex(iw, ih);
                  d = glm::dot(cloth_camera, glm::normalize(ray));
                }
              }
            }
          }
          
          if(i < cloth->local_or_world_positions.size()){
            similar_ratio = glm::length(cloth->local_or_world_positions[i] - scene.camera.transform.position) / glm::length(ray);
            old_p = new_p;
            cloth->is_fixed_masses[i] = true;
            cloth->world_velocities[i] = Vec3(0.0f, 0.0f, 0.0f);
            cloth->world_accelerations[i] = Vec3(0.0f, 0.0f, 0.0f);
          }
          cloth->WorldToLocalPositions();
        }
        
        if(i < cloth->local_or_world_positions.size()){
          cloth->LocalToWorldPositions();
          cloth->local_or_world_positions[i] += similar_ratio * (new_p - old_p);
          old_p = new_p;
          cloth->WorldToLocalPositions();
        }
        
      }else{
        if(i != 100){
          cloth->is_fixed_masses[i] = false;
          i = 100;
        }
        // if(index < cloth->local_or_world_positions.size()){
        //   cloth->is_fixed_masses[index] = false;
        //   index = 100;
        // }
      }

      if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        int w;
        int h;
        int pixel_w;
        int pixel_h;
        double x_p;
        double y_p;
        double z_p;
        glfwGetWindowSize(window, &w, &h);
        glfwGetFramebufferSize(window, &pixel_w, &pixel_h);
        glfwGetCursorPos(window, &x_p, &y_p);

        Vec2 pixel_pos = Vec2(x_p, y_p) * Vec2(pixel_w, pixel_h) / Vec2(w, h) + Vec2(0.5f, 0.5f);
        Vec3 win = Vec3(pixel_pos.x, pixel_h - pixel_pos.y, 0.0f);
        glReadPixels((GLint)win.x, (GLint)win.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &win.z);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(scene.camera.transform.position, scene.camera.transform.position + scene.camera.transform.Forward(), Vec3(0, 1, 0));
        Vec4 viewport(0.0f,0.0f,(float)window_width, (float)window_height);
        Vec3 world = glm::unProject(win, view, projection, viewport);
        Vec3 change = world - old_world;
        if(old_world != Vec3(0.0f, 0.0f, 0.0f)){
          if(glm::length(change) < 0.3){
            
            // float min_dis = 10.0f;
            // for(int i = 0; i < cloth->local_or_world_positions.size(); i++){
            //   float tmp = glm::length(world - Vec3(cloth->local_or_world_positions[i]));
            //   if(min_dis > tmp){
            //     min_dis = tmp;
            //     index = i;
            //   }
            // }
            // if(index < cloth->local_or_world_positions.size()){
            //   printf("sssssssss\n");
            //   cloth->is_fixed_masses[index] = true;
            //   cloth->local_or_world_positions[index] += change * Vec3(0.5f, 0.5f, 0.5f);
            // }

            for(int i = 0; i < cloth->local_or_world_positions.size(); i++){
              cloth->local_or_world_positions[i] += change * Vec3(0.5f, 0.5f, 0.5f);
            }
          }
        }
        old_world = world;

        
        

        // old_world = world;
        // std::cout <<"screen " << x_p <<" " << y_p <<" " << std::endl;
        // std::cout <<"change " << glm::length(change) << std::endl;
        // std::cout <<"world " << world.x <<" " << world.y <<" " << world.z << std::endl;

        // // find index 
        // int index = 100;
        // float min_dis = 10.0f; 
        // for(int i = 0; i < 12; i++){
        //     float tmp = glm::length(world - Vec3(control_plane[i]));
        //     if(min_dis > tmp){
        //         min_dis = tmp;
        //         index = i;
        //     }
        // }
        // if(index < 12)
        //     index_now = index;
        //     if(glm::length(world - control_plane[index_now]) < 1)
        //         control_plane[index_now] = world;
        // changing = 1;
      }

      /// fixed update
      for (unsigned i = 0; i < Time::fixed_update_times_this_frame; ++i) {
        if(Input::GetKey(KeyCode::Space)) { //! only when space is pressed
          scene.FixedUpdate();
        }
      }

      /// update
      {
        scene.Update();
//        printf("Pos = (%f, %f, %f)\n", scene.camera.transform.position.x, scene.camera.transform.position.y, scene.camera.transform.position.z);
//        printf("Rot = (%f, %f, %f, %f)\n", scene.camera.transform.rotation.w, scene.camera.transform.rotation.x, scene.camera.transform.rotation.y, scene.camera.transform.rotation.z);
//        printf("\n");
      }

      /// render
      {
        scene.RenderUpdate();
      }

      // swap front and back buffers
      glfwSwapBuffers(window);

      // poll for and process events
      glfwPollEvents();
    }
  }

  glfwTerminate();
  return 0;
}
