#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "texture.h"

#include<iostream>

void Texture::loadTextureFromFile(const std::string &path) {
  int w_;
  int h_;
  int ch_;
  stbi_uc *img = stbi_load(path.c_str(), &w_, &h_, &ch_, 0);
  tex_data.assign(img, img + w_ * h_ * ch_);
  stbi_image_free(img);
  weight = w_;
  height = h_;
  channel = ch_;
}

Texture::Texture(){
  type = Texture::NONE;
}

Vec3f Texture::get_tex_data(int index){
    return Vec3f((float)tex_data[index * 3], (float)tex_data[index * 3 + 1], (float)tex_data[index * 3 + 2]);
}

Vec3f Texture::bilinear_interpolation(float u, float v){
    float p_x = 0;
    float p_y = 0;
    Vec2f pos = Vec2f(u,v).cwiseProduct(Vec2f((float)weight, (float)height));

    int u0 = std::min(int(pos[0]) + 1, weight - 1);
    int v0 = std::min(int(pos[1]) + 1, height - 1);

    Vec3f left_min_3 = get_tex_data(int(pos[1]) + int(pos[0]) * height);
    Vec3f left_max_3 = get_tex_data(v0 + int(pos[0]) * height);
    Vec3f right_min_3 = get_tex_data(int(pos[1]) + u0 * height);
    Vec3f right_max_3 = get_tex_data(v0 + u0 * height);
    
    if(u0 == int(pos[0])){
        p_x = 0;
    }else if(v0 == int(pos[1])){
        p_y = 0;
    }else{
        p_x = (u0 - pos[0]) / ((float)u0 - int(pos[0]));
        p_y = (v0 - pos[1]) / ((float)v0 - int(pos[1]));
    }
    
    return ((1 - p_y) * ((1 - p_x) * left_min_3 + p_x * right_min_3) + ((1 - p_x) * left_max_3 + p_x * right_max_3) * p_y) / 255.0f;
}

Vec3f Texture::basic_interpolation(float u, float v){
    float p_x = 0;
    float p_y = 0;
    Vec2f pos = Vec2f(u,v).cwiseProduct(Vec2f((float)weight, (float)height));

    int u0 = std::min(int(pos[0]) + 1, weight - 1);
    int v0 = std::min(int(pos[1]) + 1, height - 1);
    Vec3f left_min_3 = get_tex_data(int(pos[1]) + int(pos[0]) * height);
    Vec3f left_max_3 = get_tex_data(v0 + int(pos[0]) * height);
    Vec3f right_min_3 = get_tex_data(int(pos[1]) + u0 * height);
    Vec3f right_max_3 = get_tex_data(v0 + u0 * height);

    if(u0 - pos[0] > pos[0] - int(pos[0]) && v0 - pos[1] > pos[1] - int(pos[1])){
        return left_min_3 / 255.0f;
    }
    if(u0 - pos[0] < pos[0] - int(pos[0]) && v0 - pos[1] < pos[1] - int(pos[1])){
        return right_max_3 / 255.0f;
    }
    if(u0 - pos[0] > pos[0] - int(pos[0]) && v0 - pos[1] < pos[1] - int(pos[1])){
        return left_max_3 / 255.0f;
    }
    if(u0 - pos[0] < pos[0] - int(pos[0]) && v0 - pos[1] > pos[1] - int(pos[1])){
        return right_min_3 / 255.0f;
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
    

}