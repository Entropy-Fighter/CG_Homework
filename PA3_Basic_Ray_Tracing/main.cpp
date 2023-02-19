#include <iostream>
#include <chrono>

#include "integrator.h"

#ifndef TESTBYGITHUB    // PLEASE DO NOT CHANGE
int scene_id = 1;
#else                   // PLEASE DO NOT CHANGE
int scene_id = 0;       // PLEASE DO NOT CHANGE
#endif                  // PLEASE DO NOT CHANGE
void setSceneById(std::shared_ptr<Scene> &scene, int id);

int main(int argc, char *argv[]) {
#ifndef TESTBYGITHUB                    // PLEASE DO NOT CHANGE
  Vec2i img_resolution(400, 400);
#else                                   // PLEASE DO NOT CHANGE
    Vec2i img_resolution(32, 32);     // PLEASE DO NOT CHANGE
#endif                                  // PLEASE DO NOT CHANGE
  Vec3f cam_pos(0, -3, 1.3);
  Vec3f cam_look_at(0, 0, 1);

  // init image.
  std::shared_ptr<ImageRGB> rendered_img
      = std::make_shared<ImageRGB>(img_resolution.x(), img_resolution.y());
  // camera setting
  std::shared_ptr<Camera> camera = std::make_shared<Camera>();
  camera->setPosition(cam_pos);
  
  camera->lookAt(cam_look_at);
  if(scene_id == 2){
    camera->setPosition(Vec3f(0, -3, 1.3));
    camera->lookAt(Vec3f(-1, -1, 1));
  }
  camera->setFov(45);
  camera->setImage(rendered_img);
  // construct scene.
  std::shared_ptr<Scene> scene = std::make_shared<Scene>();
  setSceneById(scene, scene_id);

  std::unique_ptr<PhongLightingIntegrator> integrator
      = std::make_unique<PhongLightingIntegrator>(camera, scene);
  std::cout << "Start Rendering..." << std::endl;
  auto start = std::chrono::steady_clock::now();
  // render scene
  integrator->render();
  auto end = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "\nRender Finished in " << time << "ms." << std::endl;
#ifndef TESTBYGITHUB                                    // PLEASE DO NOT CHANGE
  rendered_img->writeImgToFile("../result.png");
#else                                                   // PLEASE DO NOT CHANGE
    rendered_img->writeImgToFile("../submit.png");      // PLEASE DO NOT CHANGE
#endif                                                  // PLEASE DO NOT CHANGE
  std::cout << "Image saved to disk." << std::endl;
  return 0;
}

void setSceneById(std::shared_ptr<Scene> &scene, int id) {
  Vec3f light_pos(0, 0, 1.95f);
  Vec3f light_color(1, 1, 1);
  Vec2f light_dim(0.5, 0.3);
  Vec3f light_n(0, 0, -1);
  Vec3f light_t(1, 0, 0);
  std::shared_ptr<Light>
      square_light = std::make_shared<SquareAreaLight>(light_pos, light_color, light_dim, light_n, light_t);

  std::shared_ptr<Material>
      mat_white =
      std::make_shared<ConstColorMat>(Vec3f(0.7f, 0.7f, 0.7f));
  std::shared_ptr<Material>
      mat_red =
      std::make_shared<ConstColorMat>(Vec3f(0.8f, 0.f, 0.f));
  std::shared_ptr<Material>
      mat_green =
      std::make_shared<ConstColorMat>(Vec3f(0.f, 0.8f, 0.f));
  std::shared_ptr<Material>
      mat_yellow =
      std::make_shared<ConstColorMat>(Vec3f(1, 0.93, 0.6));
  std::shared_ptr<Material>
      mat_blue =
      std::make_shared<ConstColorMat>(Vec3f(0.2, 0.5, 0.9));

  std::shared_ptr<Geometry>
      floor = std::make_shared<Rectangle>(Vec3f(0, 0, 0), Vec2f(2, 2), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
  std::shared_ptr<Geometry>
      ceiling = std::make_shared<Rectangle>(Vec3f(0, 0, 2), Vec2f(2, 2), Vec3f(0, 0, -1), Vec3f(1, 0, 0));
  std::shared_ptr<Geometry>
      wall_left = std::make_shared<Rectangle>(Vec3f(-1, 0, 1), Vec2f(2, 2), Vec3f(1, 0, 0), Vec3f(0, 0, 1));
  std::shared_ptr<Geometry>
      wall_right = std::make_shared<Rectangle>(Vec3f(1, 0, 1), Vec2f(2, 2), Vec3f(-1, 0, 0), Vec3f(0, 0, -1));
  std::shared_ptr<Geometry>
      wall_back = std::make_shared<Rectangle>(Vec3f(0, 1, 1), Vec2f(2, 2), Vec3f(0, -1, 0), Vec3f(1, 0, 0));

  TextureMat mat_texture;
//   mat_texture.set_disp_map("../textures/disp.jpg");
//   mat_texture.set_norm_map("../textures/norm.jpg");
  std::shared_ptr<Material> tex;
  if(scene_id == 1){
    mat_texture.norm_map("D://Documents//GitHub//assignment-3-codingnoooob//textures//norm.jpg");
    mat_texture.diff_map("D://Documents//GitHub//assignment-3-codingnoooob//textures//diff.jpg");
    tex = std::make_shared<TextureMat>(mat_texture);
  }
  switch (id) {
    case 0 : {
      floor->setMaterial(mat_white);
      ceiling->setMaterial(mat_white);
      wall_back->setMaterial(mat_white);
      wall_left->setMaterial(mat_red);
      wall_right->setMaterial(mat_green);
      scene->addGeometry(floor);
      scene->addGeometry(ceiling);
      scene->addGeometry(wall_left);
      scene->addGeometry(wall_right);
      scene->addGeometry(wall_back);
      addTetrahedronToScene(scene,
                            mat_blue,
                            Vec3f(-1, 0.2, 0.2),
                            Vec3f(-0.5, -0.65, 0.2),
                            Vec3f(0, 0.2, 0.2),
                            Vec3f(-0.5, -0.3774, 1));
      addEllipsoidToScene(scene,
                          mat_yellow,
                          Vec3f(0.5, 0.4, 0.4),
                          Vec3f(0.3, 0.1, 0),
                          Vec3f(-0.1, 0.3, 0),
                          Vec3f(0, 0, 0.4));
      scene->setLight(square_light);
      scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
      break;
    }
    case 1: {
      // UNIMPLEMENTED
      // You can add your custom scene here.
      //   floor->setMaterial(mat_white);
      floor->setMaterial(tex);
      ceiling->setMaterial(mat_white);
      wall_back->setMaterial(tex);
      wall_left->setMaterial(mat_red);
      wall_right->setMaterial(mat_green);
      scene->addGeometry(floor);
      scene->addGeometry(ceiling);
      scene->addGeometry(wall_left);
      scene->addGeometry(wall_right);
      scene->addGeometry(wall_back);
      addTetrahedronToScene(scene,
                            mat_blue,
                            Vec3f(-1, 0.2, 0.2),
                            Vec3f(-0.5, -0.65, 0.2),
                            Vec3f(0, 0.2, 0.2),
                            Vec3f(-0.5, -0.3774, 1));
      addEllipsoidToScene(scene,
                          mat_yellow,
                          Vec3f(0.5, 0.4, 0.4),
                          Vec3f(0.3, 0.1, 0),
                          Vec3f(-0.1, 0.3, 0),
                          Vec3f(0, 0, 0.4));
      scene->setLight(square_light);
      scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
      break;
    }
    case 2: {
      // UNIMPLEMENTED
      // You can add your custom scene here.
      //   floor->setMaterial(mat_white);

      
      GroundMat mat_texture;
      mat_texture.grid_map("D://Documents//GitHub//assignment-3-codingnoooob//textures//grid.png");
      std::shared_ptr<Material> tex_grid = std::make_shared<GroundMat>(mat_texture);
      
      std::shared_ptr<Geometry> g = std::make_shared<Ground>(Vec3f(0, 0, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g1 = std::make_shared<Ground>(Vec3f(0, 8, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g1->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g2 = std::make_shared<Ground>(Vec3f(0, 16, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g2->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g3 = std::make_shared<Ground>(Vec3f(0, 24, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g3->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g4 = std::make_shared<Ground>(Vec3f(-8, 24, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g4->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g5 = std::make_shared<Ground>(Vec3f(8, 24, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g5->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g6 = std::make_shared<Ground>(Vec3f(-8, 16, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g6->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g7 = std::make_shared<Ground>(Vec3f(8, 16, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g7->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g8 = std::make_shared<Ground>(Vec3f(-8, 8, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g8->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g9 = std::make_shared<Ground>(Vec3f(8, 8, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g9->setMaterial(tex_grid);
      std::shared_ptr<Geometry> g10 = std::make_shared<Ground>(Vec3f(-8, 0, 0), Vec2f(8, 8), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
      g10->setMaterial(tex_grid);
      
      scene->addGeometry(g);
      scene->addGeometry(g1);
      scene->addGeometry(g2);
      scene->addGeometry(g3);
      scene->addGeometry(g4);
      // scene->addGeometry(g5);
      scene->addGeometry(g6);
      // scene->addGeometry(g7);
      scene->addGeometry(g8);
      // scene->addGeometry(g9);
      scene->addGeometry(g10);
      scene->setLight(square_light);
      scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
      break;
    }
    default:break;
  }
  

}
