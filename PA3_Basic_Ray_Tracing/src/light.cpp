#include "light.h"

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(const Vec3f &pos, const Vec3f &color) :
    position(pos), color(color) {

}
Vec3f Light::getColor() const {
  return color;
}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &dimension,
                                 const Vec3f &normal,
                                 const Vec3f &tangent) :
    Light(pos, color),
    rectangle(pos, dimension, normal, tangent) {}

std::vector<LightSample> SquareAreaLight::samples() const {
  std::vector<LightSample> samples;
  // TODO: Your code here.
  Vec2f size_rectangle = rectangle.getSize();
  Vec3f tangent_rectangle = rectangle.getTangent();
  Vec3f cotangent_rectangle = rectangle.getNormal().cross(tangent_rectangle).normalized();
  for(int i = 0; i < 16; i++){
      for(int j = 0; j < 16; j++){
          float x = (2 * ((float)i + 0.5f) / SAMPLE_NUM - 1) * size_rectangle[0] * 0.5;
          float y = (2* ((float)j + 0.5f) / 16 - 1) * size_rectangle[1] * 0.5;
          LightSample sample;
          sample.color = color;
          sample.position = position + tangent_rectangle * x + cotangent_rectangle * y;
          samples.push_back(sample);
      }
      
  }
  return samples;
}
bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: Your code here.
  bool intersection = false;
  if(rectangle.intersect(ray, interaction)){
    intersection = true;
    interaction.type = Interaction::Type::LIGHT;
    return intersection;
  }
  return intersection;
}

