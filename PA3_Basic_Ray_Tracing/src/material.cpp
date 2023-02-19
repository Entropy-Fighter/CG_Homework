#include "material.h"

#include <utility>

ConstColorMat::ConstColorMat() : diffuse_color({1, 1, 1}),
                                 specular_color({1, 1, 1}),
                                 ambient_color(1, 1, 1),
                                 shininess(16.f) {}

ConstColorMat::ConstColorMat(const Vec3f &color, float sh)
    : diffuse_color(color), specular_color(color), ambient_color(color), shininess(sh) {}

ConstColorMat::ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh)
    : diffuse_color(std::move(diff)),
      specular_color(std::move(spec)),
      ambient_color(std::move(ambient)),
      shininess(sh) {}

InteractionPhongLightingModel ConstColorMat::evaluate(Interaction &interaction) const {
  InteractionPhongLightingModel m;
  // TODO: Your code here.
  m.ambient = ambient_color;
  m.diffusion = diffuse_color;
  m.specular = specular_color;
  m.shininess = shininess;

  return m;
}

InteractionPhongLightingModel TextureMat::evaluate(Interaction &interaction) const {
  InteractionPhongLightingModel m;
  m.ambient = diff_t->bilinear_interpolation(interaction.uv[0], interaction.uv[1]);
  m.diffusion = diff_t->bilinear_interpolation(interaction.uv[0], interaction.uv[1]);
  m.shininess = 16.0f;
  m.specular = diff_t->bilinear_interpolation(interaction.uv[0], interaction.uv[1]); 
  return m;
}

InteractionPhongLightingModel GroundMat::evaluate(Interaction &interaction) const {
  InteractionPhongLightingModel m;
  m.ambient = grid_t->basic_interpolation(interaction.uv[0], interaction.uv[1]);
  m.diffusion = grid_t->basic_interpolation(interaction.uv[0], interaction.uv[1]);
  m.shininess = 16.0f;
  m.specular = grid_t->basic_interpolation(interaction.uv[0], interaction.uv[1]); 
  return m;
}

void TextureMat::disp_map(const std::string &disp_path){
  disp_t->loadTextureFromFile(disp_path);
  disp_t->type = Texture::DISP;
}

void TextureMat::diff_map(const std::string &diff_path){
  diff_t->loadTextureFromFile(diff_path);
  diff_t->type = Texture::DIFF;
}

void TextureMat::norm_map(const std::string &norm_path){
  norm_t->loadTextureFromFile(norm_path);
  norm_t->type = Texture::NORM;
}

void TextureMat::grid_map(const std::string &grid_path){
  grid_t->loadTextureFromFile(grid_path);
  grid_t->type = Texture::GRID;
}

void GroundMat::grid_map(const std::string &grid_path){
  grid_t->loadTextureFromFile(grid_path);
  grid_t->type = Texture::GRID;
}

TextureMat::TextureMat(){
  diff_t = new Texture();
  norm_t = new Texture();
  disp_t = new Texture();  
  grid_t = new Texture(); 
}

GroundMat::GroundMat(){ 
  grid_t = new Texture(); 
}

