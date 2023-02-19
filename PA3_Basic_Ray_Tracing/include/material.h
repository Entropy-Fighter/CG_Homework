#ifndef CS171_HW3_INCLUDE_MATERIAL_H_
#define CS171_HW3_INCLUDE_MATERIAL_H_

#include "interaction.h"
#include "texture.h"

class Material {
 public:
  Material(){
    diff_t = nullptr; 
    disp_t = nullptr;
    norm_t = nullptr;
    grid_t = nullptr;
  }
  virtual ~Material() = default;
  [[nodiscard]] virtual InteractionPhongLightingModel evaluate(Interaction &interaction) const = 0;
  
  Texture *disp_t;
  Texture *norm_t;
  Texture *diff_t;
  Texture *grid_t;
};


class ConstColorMat : public Material {
 public:
  ConstColorMat();
  explicit ConstColorMat(const Vec3f &color, float sh = 16.f);
  ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh = 16.f);
  [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
 private:
  Vec3f diffuse_color;
  Vec3f specular_color;
  Vec3f ambient_color;
  float shininess;
};

class TextureMat : public Material {
 // TODO: Bonus - texture material
 public:
  TextureMat();
  void disp_map(const std::string &disp_path);
  void diff_map(const std::string &diff_path);
  void norm_map(const std::string &norm_path);
  void grid_map(const std::string &grid_path);
  [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
};

class GroundMat : public Material {
 // TODO: Bonus - texture material
 public:
  GroundMat();
  void grid_map(const std::string &grid_path);
  [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
};

#endif //CS171_HW3_INCLUDE_MATERIAL_H_
