#include "light.h"
#include "utils.h"

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(const Vec3f &pos, const Vec3f &color) :
    position(pos), radiance(color) {}

Vec3f Light::getRadiance(){
  return radiance; 
}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &size) :
    Light(pos, color), size(size) {
  Vec3f v1, v2, v3, v4;
  v1 = pos + Vec3f(size.x() / 2, 0.f, -size.y() / 2);
  v2 = pos + Vec3f(-size.x() / 2, 0.f, -size.y() / 2);
  v3 = pos + Vec3f(-size.x() / 2, 0.f, size.y() / 2);
  v4 = pos + Vec3f(size.x() / 2, 0.f, size.y() / 2);
  light_mesh =TriangleMesh({v1, v2, v3, v4}, {Vec3f(0, -1, 0)}, {0, 1, 2, 0, 2, 3}, {0,0,0,0,0,0});
}

Vec3f SquareAreaLight::emission(const Vec3f &pos, const Vec3f &dir) const {
  // return {0,0,0};
  return std::max(0.0f, dir.dot(Vec3f(0, -1, 0))) * radiance;
}

float SquareAreaLight::pdf(const Interaction &interaction, Vec3f pos) {
  float cos= std::max(0.0f, -interaction.wo.dot(Vec3f(0, -1, 0)));
  float distance = (pos - interaction.pos).norm();
  return cos / distance / distance;
}


Vec3f SquareAreaLight::sample(Interaction &interaction, float *pdf, Sampler &sampler) const {
  // return {0,0,0};
  Vec2f s = sampler.get2D();
  Vec3f pos = position + Vec3f((s[0] - 0.5f) * size[0], 0.0f, size[1] * (s[1] - 0.5f));
  *pdf = 1 / float(size[0] * size[1]);
  interaction.wo = (pos - interaction.pos).normalized();
  return pos;
}

bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
  if (light_mesh.intersect(ray, interaction)) {
    interaction.type = Interaction::Type::LIGHT;
    return true;
  }
  return false;
}

