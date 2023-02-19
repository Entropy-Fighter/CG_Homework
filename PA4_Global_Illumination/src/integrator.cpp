#include "integrator.h"
#include "utils.h"
#include <omp.h>

#include <utility>

Integrator::Integrator(std::shared_ptr<Camera> cam,
                       std::shared_ptr<Scene> scene, int spp, int max_depth)
    : camera(std::move(cam)), scene(std::move(scene)), spp(spp), max_depth(max_depth) {
}

void Integrator::render() const {
  Vec2i resolution = camera->getImage()->getResolution();
  int cnt = 0;
  Sampler sampler;
  float sample_num = 16;
#pragma omp parallel for schedule(dynamic), shared(cnt), private(sampler)
  for (int dx = 0; dx < resolution.x(); dx++) {
#pragma omp atomic
    ++cnt;
    printf("\r%.02f%%", cnt * 100.0 / resolution.x());
    sampler.setSeed(omp_get_thread_num());
    for (int dy = 0; dy < resolution.y(); dy++) {
      Vec3f L(0, 0, 0);
      // TODO: generate #spp rays for each pixel and use Monte Carlo integration to compute radiance.
      for(float i = 0; i < sample_num; i++){
        for(float j = 0; j < sample_num; j++){
          Ray ray_tmp = camera->generateRay(dx + i / sample_num, dy + j / sample_num);
          L += radiance(ray_tmp, sampler);
        }
      }
      L = L / sample_num / sample_num;

      camera->getImage()->setPixel(dx, dy, L);
    }
  }
}

Vec3f Integrator::radiance(Ray &ray, Sampler &sampler) const {
  Vec3f L(0, 0, 0);
  Vec3f beta(1, 1, 1);
  bool isDelta = false;
  for (int i = 0; i < max_depth; ++i) {
    /// Compute radiance (direct + indirect)
    Interaction interaction;
    bool intersect = scene->intersect(ray, interaction);
    interaction.wi = ray.direction;
    if(!intersect){
      break;
    }else{
      if(interaction.type == Interaction::Type::LIGHT){
        if(i == 0){
          L = scene->getLight()->emission(interaction.pos, -interaction.wi);
          // L = scene->getLight()->getRadiance();
        }else
          break;
      }else if(interaction.type == Interaction::Type::GEOMETRY){
        if(interaction.material->isDelta()){
          Interaction r_interaction;
          float pdf = interaction.material->sample(interaction, sampler);
          ray = Ray(interaction.pos + 0.001 * interaction.wo, interaction.wo);
          bool r_intersect = scene->intersect(ray, r_interaction);
          r_interaction.wi = ray.direction;

          if(r_intersect && r_interaction.type == Interaction::Type::LIGHT && (i == 0)){
            Vec3f _light = scene->getLight()->emission(r_interaction.pos, -r_interaction.wi);
            L += beta.cwiseProduct(_light.cwiseProduct(interaction.material->evaluate(interaction)));
            break;
          }
          beta = beta.cwiseProduct(interaction.material->evaluate(interaction));
        }else{
          Vec3f direct = directLighting(interaction, sampler);
          float pdf = interaction.material->sample(interaction, sampler);
          L += beta.cwiseProduct(direct); 
          beta = beta.cwiseProduct(interaction.material->evaluate(interaction)) * interaction.wo.dot(interaction.normal) / pdf;
          ray = Ray(interaction.pos + interaction.wo * 0.001, interaction.wo);
        }
      }
    }
  }
  return L;
}


Vec3f Integrator::directLighting(Interaction &interaction, Sampler &sampler) const {
  Vec3f L(0, 0, 0);
  // Compute direct lighting.
  float pdf;
  Vec3f pos = scene->getLight()->sample(interaction, &pdf, sampler);
  Ray ray = Ray(interaction.pos + interaction.wo * 0.001, interaction.wo);

  if(scene->isShadowed(ray)){
    return L;
  }else{
    L = scene->getLight()->emission(pos, -interaction.wo).cwiseProduct(interaction.material->evaluate(interaction)) * interaction.wo.dot(interaction.normal) * scene->getLight()->pdf(interaction, pos) / pdf;
    return L;
  }

  return L;
}