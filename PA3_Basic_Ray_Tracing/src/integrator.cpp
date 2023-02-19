#include "integrator.h"
#include <omp.h>
#include <iostream>
#include <utility>

PhongLightingIntegrator::PhongLightingIntegrator(std::shared_ptr<Camera> cam,
                                                 std::shared_ptr<Scene> scene)
    : camera(std::move(cam)), scene(std::move(scene)) {
}

void PhongLightingIntegrator::render() const {
  Vec2i resolution = camera->getImage()->getResolution();
  int cnt = 0;
  std::vector<LightSample> light_samples = scene->getLight()->samples();
  // srand(0);
  int sample_num = 20;
  #pragma omp parallel for schedule(guided, 2), shared(cnt)
  for(int dx = 0; dx < resolution.x(); dx++){
    #pragma omp atomic
    ++cnt;
    printf("\r%.02f%%", cnt * 100.0 / resolution.x());
    for(int dy = 0; dy < resolution.y(); dy++){
      Vec3f L(0.0f, 0.0f, 0.0f);
      // TODO: Your code here.
      // std::vector<Vec2f> pixel_samples = camera->sample_pixel_uniform(dx, dy, 2);
      // std::vector<Vec2f> pixel_samples = camera->sample_pixel(dx, dy, 7);
      std::vector<Vec2f> pixel_samples = camera->sample_pixel(dx, dy, 2);
      // std::vector<Vec2f> pixel_samples;
      // pixel_samples.push_back(Vec2f(dx, dy));
      for(Vec2f pixel_point : pixel_samples){
				Ray ray = camera->generateRay(pixel_point[0], pixel_point[1]);
				Interaction interaction;
        Vec3f ray_tmp(0.0f, 0.0f, 0.0f);
        if(scene->intersect(ray, interaction)){
          if(interaction.type == Interaction::Type::LIGHT){
            ray_tmp = scene->getLight()->getColor();
          }else{
            for(auto light:light_samples){
              Ray r(interaction.pos, (light.position - interaction.pos).normalized());
              Interaction pos_light;
              if(scene->notShadowed(r)) {
                ray_tmp += radiance(ray, interaction, light) / light_samples.size();
              }else{
                ray_tmp += interaction.model.ambient.cwiseProduct(scene->getAmbient()) / light_samples.size();
              }
            }
          }
          L = L + ray_tmp;
        }
			}
      L = L / pixel_samples.size();
      camera->getImage()->setPixel(dx, dy, L);
    }
  }
}



Vec3f PhongLightingIntegrator::radiance(Ray &ray, Interaction &interaction, LightSample light) const {
  Vec3f radiance(0.0f, 0.0f, 0.0f);
 
  Vec3f light_direction = (light.position - interaction.pos).normalized();
  Vec3f normal_ = interaction.normal.normalized();
  float diffuse_factor = std::max(0.0f, light_direction.dot(normal_));

  // Vec3f direction = (light_direction - ray.direction).normalized();
  // float specular_factor = std::pow(std::max(0.0f, direction.dot(normal_)), interaction.model.shininess);
  Vec3f reflect_direction = (2 * light_direction.dot(interaction.normal.normalized()) * interaction.normal.normalized() -light_direction).normalized();
  float specular_factor = std::pow(std::max(0.0f, reflect_direction.dot(-ray.direction.normalized())), interaction.model.shininess);

  Vec3f ds_light_factor = diffuse_factor * interaction.model.diffusion + specular_factor * interaction.model.specular;
  radiance = scene->getAmbient().cwiseProduct(interaction.model.ambient) + (ds_light_factor.cwiseProduct(light.color));

  return radiance;    
}
