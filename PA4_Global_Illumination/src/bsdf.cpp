#include "bsdf.h"
#include "utils.h"

#include <utility>

IdealDiffusion::IdealDiffusion(const Vec3f &color) : color(color) {}

Vec3f IdealDiffusion::evaluate(Interaction &interaction) const {
  // TODO: your implementation here.
  return INV_PI * color;
  // return {0,0,0};
}

float IdealDiffusion::pdf(Interaction &interaction) const {
  // TODO: your implementation here
  return  INV_PI * interaction.wo.dot(interaction.normal); 
  // return 0;
}

float IdealDiffusion::sample(Interaction &interaction, Sampler &sampler) const {
  Vec2f eta = sampler.get2D();
  float theta = asin(sqrt(eta[0]));
  float phi = 2 * PI * eta[1];
  Vec3f direction(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
  Mat3f trans = Eigen::Quaternionf::FromTwoVectors(Vec3f(0.0f, 0.0f, 1.0f), interaction.normal).toRotationMatrix();
  Vec3f res = trans * direction;
  interaction.wo = res.normalized();
  return pdf(interaction);
}
/// return whether the bsdf is perfect transparent or perfect reflection
bool IdealDiffusion::isDelta() const {
  return false;
}


IdealSpecular::IdealSpecular(const Vec3f &color) : color(color) {}
Vec3f IdealSpecular::evaluate(Interaction &interaction) const {
  return color;
}
float IdealSpecular::pdf(Interaction &interaction) const {
  return 1.0f;
}
float IdealSpecular::sample(Interaction &interaction, Sampler &sampler) const {
  interaction.wo = (2 * (-interaction.wi).dot(interaction.normal) * interaction.normal + interaction.wi).normalized();
	return pdf(interaction);
}

bool IdealSpecular::isDelta() const {
  return true;
}

//Reference: https://www.pbr-book.org/3ed-2018/Reflection_Models/Specular_Reflection_and_Transmission
float FrDielectric(float cosThetaI, float etaI, float etaT) {
  cosThetaI = std::clamp(cosThetaI, -1.0f, 1.0f);
  if(cosThetaI < 0.0f){
    std::swap(etaI, etaT);
    cosThetaI = std::abs(cosThetaI);
  }
  float sinThetaI = std::sqrt(std::max(0.0f, 1 - cosThetaI * cosThetaI));
  float sinThetaT = etaI / etaT * sinThetaI;
  float cosThetaT = std::sqrt(std::max(0.0f, 1 - sinThetaT * sinThetaT));
  float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
  float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
  return (Rparl * Rparl + Rperp * Rperp) / 2;
}
bool Refract(const Vec3f &wi, const Vec3f &n, float eta, Vec3f *wo) {
  float cosThetaI = std::abs(n.dot(wi));
  float sin2ThetaI = std::max(0.0f, 1.0f - cosThetaI * cosThetaI);
  float sin2ThetaT = eta * eta * sin2ThetaI;
  if(sin2ThetaT >= 1.0f){
    return false;
  }
  float cosThetaT = std::sqrt(1 - sin2ThetaT);
  *wo = (( eta * cosThetaI -cosThetaT) * n + eta * (-wi)).normalized();
  return true;
}

Translucent::Translucent(const Vec3f &color) : color(color) {}
Translucent::Translucent(const Vec3f &color, float eta_in, float eta_out) : color(color), eta_in(eta_in), eta_out(eta_out) {}
Vec3f Translucent::evaluate(Interaction &interaction) const {
  Vec3f reflect = (2 * (-interaction.wi).dot(interaction.normal) * interaction.normal + interaction.wi).normalized();
  if(interaction.wo.dot(reflect) >= 0.999){
    return color;
  }else{
    float etaI;
    float etaT;
    if((-interaction.wi).dot(interaction.normal) > 0){
      etaI = eta_in;
      etaT = eta_out;
    }else{
      etaI = eta_out;
      etaT = eta_in;
    }
    return etaI * etaI / (etaT * etaT) * (1 - FrDielectric((-interaction.wi).dot(interaction.normal), etaI, etaT)) * color;
  }
}

float Translucent::pdf(Interaction &interaction) const{
  return 1.0f;
}

float Translucent::sample(Interaction &interaction, Sampler &sampler) const {
  float etaI;
  float etaT;
  if((-interaction.wi).dot(interaction.normal) > 0){
    etaI = eta_in;
    etaT = eta_out;
  }else{
    etaI = eta_out;
    etaT = eta_in;
  }
  if(!Refract(-interaction.wi, interaction.normal, etaI / etaT, &interaction.wo)){
    interaction.wo = (2 * (-interaction.wi).dot(interaction.normal) * interaction.normal + interaction.wi).normalized();
  }
  return pdf(interaction);
}
bool Translucent::isDelta() const {
  return true;
}