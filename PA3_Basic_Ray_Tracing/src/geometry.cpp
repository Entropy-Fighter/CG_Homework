#include "geometry.h"

#include <utility>

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2)
    : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)) {
  normal = (v1 - v0).cross(v2 - v1).normalized();
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: Your code here.
 
  Vec3f v0_v1 = v1 - v0;
  Vec3f v0_v2 = v2 - v0;
  Vec3f direction = ray.direction;
  Vec3f origin = ray.origin;
  Vec3f s = origin - v0;
  Vec3f s1 = direction.cross(v0_v2);
  Vec3f s2 = s.cross(v0_v1);

  float s1_dot_v0v1 = v0_v1.dot(s1);
  float s1_dot_s = s1.dot(s);
  float s2_dot_d = s2.dot(direction);
  float s2_dot_v0v2 = s2.dot(v0_v2);

  if(fabs(s1_dot_v0v1) < 0.0001){
    return false;
  }
  float b1 = s1_dot_s / s1_dot_v0v1;
  float b2 = s2_dot_d / s1_dot_v0v1;
  float t = s2_dot_v0v2 / s1_dot_v0v1;
  if(t < 0.0001) 
    return false;
  if(b1 >= 0 && b2 >= 0 && b1 + b2 <=1){
    if(t < interaction.dist){
      interaction.dist = t;
      interaction.pos = ray.operator()(t);
      interaction.normal = normal;

      // uv
      
      if(material != nullptr){
        interaction.model = material->evaluate(interaction);
      }
      interaction.type = Interaction::Type::GEOMETRY;
      return true; 
    }
  } 

  return false;
}


Rectangle::Rectangle(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent)
    : Geometry(),
      position(std::move(position)),
      size(std::move(dimension)),
      normal(std::move(normal)),
      tangent(std::move(tangent)) {
        normal = normal.normalized();
        tangent = tangent.normalized();
      }

bool Rectangle::intersect(Ray &ray, Interaction &interaction) const {
  // // TODO: Your code here
  // if(ray.direction.dot(normal) == 0){
  //   return false;
  // }
  
  // float t = (position - ray.origin).dot(normal) / ray.direction.dot(normal);
  // Eigen::Vector3f p0_p = ray.operator()(t) - position;
  // float f0 = fabs(p0_p.dot(tangent) / tangent.norm());
  // float f1 = fabs(p0_p.dot(tangent.cross(normal)) / tangent.cross(normal).norm());

  // ConstColorMat color_mat;

  // if(f0 <= size[0] / 2 && f1 <= size[1] / 2 && t >= ray.t_min && t <= ray.t_max){
  //   if(t < interaction.dist){
  //     interaction.dist = t;
  //     interaction.normal = normal;
  //     interaction.pos = ray.operator()(t);
  //     // interaction.uv[0] = f0;
  //     // interaction.uv[1] = f1;
  //     if(material != nullptr){
  //       interaction.model = material->evaluate(interaction);
  //     }
  //     interaction.type = Interaction::Type::GEOMETRY;
  //     return true;
  //   }
  // }
  // return false;

  // TODO: Your code here
  if(ray.direction.dot(normal) == 0){
    return false;
  }
  
  ConstColorMat color_mat;

  float t = (position - ray.origin).dot(normal) / ray.direction.dot(normal);
  Eigen::Vector3f p0_p = ray.operator()(t) - position;
  float f0 = fabs(p0_p.dot(tangent) / tangent.norm());
  float f1 = fabs(p0_p.dot(tangent.cross(normal)) / tangent.cross(normal).norm()); 

  if(f0 <= size[0] / 2 && f1 <= size[1] / 2 && t >= ray.t_min && t <= ray.t_max){
    if(t < interaction.dist){
      interaction.dist = t;
      interaction.normal = normal;
      interaction.pos = ray.operator()(t);
      interaction.uv[0] = p0_p.dot(tangent) / tangent.norm() / (size[0]) + 0.5f;
      interaction.uv[1] = p0_p.dot(tangent.cross(normal)) / tangent.cross(normal).norm() / (size[1]) + 0.5f;
      interaction.type = Interaction::Type::GEOMETRY;
      if(material){
        interaction.model = material->evaluate(interaction);
        if(material->norm_t){
          Vec2f pos;
          Vec3f local_normal;
          pos = Vec2f(interaction.uv[0], interaction.uv[1]).cwiseProduct(Vec2f((float)material->norm_t->weight, (float)material->norm_t->height));
          local_normal = material->norm_t->get_tex_data((int)pos[1] + (int)pos[0] * material->norm_t->height);
          Vec3f final_normal = local_normal[2] * normal + local_normal[0] * tangent.normalized() + local_normal[1] * tangent.normalized().cross(normal.normalized());
          interaction.normal = final_normal.normalized();
        }
        // else{
        //   interaction.normal = normal;
        // }
        // interaction.normal = normal;
        interaction.model = material->evaluate(interaction);
      }else{
        interaction.model = color_mat.evaluate(interaction);
      }
      interaction.type = Interaction::Type::GEOMETRY;
      return true;
    }
  }
  return false;
}

Vec2f Rectangle::getSize() const {
  return size;
}
Vec3f Rectangle::getNormal() const {
  return normal;
}
Vec3f Rectangle::getTangent() const {
  return tangent;
}

Ellipsoid::Ellipsoid(const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c)
    : p(p), a(a), b(b), c(c) {}

bool Ellipsoid::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: Your code here.
 
  Eigen::Matrix4f T;
  Eigen::Matrix4f R;
  Eigen::Matrix4f S;
  T << 1.0f, 0.0f, 0.0f, p[0], 0.0f, 1.0f, 0.0f, p[1], 0.0f, 0.0f, 1.0f, p[2], 0.0f, 0.0f, 0.0f, 1.0f;
  R << a.normalized()[0], b.normalized()[0], c.normalized()[0], 0.0f, a.normalized()[1], b.normalized()[1], c.normalized()[1], 0.0f, a.normalized()[2], b.normalized()[2], c.normalized()[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f;
  S << a.norm(), 0.0f, 0.0f, 0.0f, 0.0f, b.norm(), 0.0f, 0.0f, 0.0f, 0.0f, c.norm(), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f;
  
  
  Eigen::Matrix4f inverse_ = (T * R * S).inverse();

  Vec4f direction_vec4 = inverse_ * Vec4f(ray.direction[0], ray.direction[1], ray.direction[2], 0.0f);
  Vec4f origin_vec4 = inverse_ * Vec4f(ray.origin[0], ray.origin[1], ray.origin[2], 1.0f);
  Vec3f origin_ = Vec3f(origin_vec4[0], origin_vec4[1], origin_vec4[2]);
  Vec3f direction_ = Vec3f(direction_vec4[0], direction_vec4[1], direction_vec4[2]);
  float d_l = float(direction_.norm());
  direction_ /= float(direction_.norm());

  Vec3f L = Vec3f(0.0f, 0.0f, 0.0f) - origin_;
  float tca = L.dot(direction_);
  if(tca >= 0.0f){
    float d_square = L.dot(L) - std::pow(tca, 2.0f);
    if(d_square <= 1.0f){ // r = 1
      float tnc = std::sqrt(1.0f - d_square);
      float t = tca - tnc;
      if(t / d_l < interaction.dist){
        interaction.dist = t / d_l;
        interaction.normal = Vec3f((inverse_.transpose() * Vec4f((origin_ + t * direction_)[0], (origin_ + t * direction_)[1], (origin_ + t * direction_)[2], 0.0f))[0], 
                                    (inverse_.transpose() * Vec4f((origin_ + t * direction_)[0], (origin_ + t * direction_)[1], (origin_ + t * direction_)[2], 0.0f))[1],
                                    (inverse_.transpose() * Vec4f((origin_ + t * direction_)[0], (origin_ + t * direction_)[1], (origin_ + t * direction_)[2], 0.0f))[2]);
        interaction.pos = ray.origin + t / d_l * ray.direction;
        interaction.model = material->evaluate(interaction);
        interaction.type = Interaction::Type::GEOMETRY;
        return true;
      }   
    }
  }
  return false;
}






bool Ground::intersect(Ray &ray, Interaction &interaction) const {
  if(ray.direction.dot(normal) == 0){
    return false;
  }
  

  float t = (position - ray.origin).dot(normal) / ray.direction.dot(normal);
  Eigen::Vector3f p0_p = ray.operator()(t) - position;
  float f0 = fabs(p0_p.dot(tangent) / tangent.norm());
  float f1 = fabs(p0_p.dot(tangent.cross(normal)) / tangent.cross(normal).norm()); 

  if(f0 <= size[0] / 2 && f1 <= size[1] / 2 && t >= ray.t_min && t <= ray.t_max){
    if(t < interaction.dist){
      interaction.dist = t;
      interaction.normal = normal;
      interaction.pos = ray.operator()(t);
      interaction.uv[0] = p0_p.dot(tangent) / tangent.norm() / (size[0]) + 0.5f;
      interaction.uv[1] = p0_p.dot(tangent.cross(normal)) / tangent.cross(normal).norm() / (size[1]) + 0.5f;
      interaction.type = Interaction::Type::GEOMETRY;
      if(material){
        interaction.model = material->evaluate(interaction);
        if(material->norm_t){
          Vec2f pos;
          Vec3f local_normal;
          pos = Vec2f(interaction.uv[0], interaction.uv[1]).cwiseProduct(Vec2f((float)material->norm_t->weight, (float)material->norm_t->height));
          local_normal = material->norm_t->get_tex_data((int)pos[1] + (int)pos[0] * material->norm_t->height);
          Vec3f final_normal = local_normal[2] * normal + local_normal[0] * tangent.normalized() + local_normal[1] * tangent.normalized().cross(normal.normalized());
          interaction.normal = final_normal.normalized();
        }
        // else{
        //   interaction.normal = normal;
        // }
        // interaction.normal = normal;
        // interaction.model = material->evaluate(interaction);
      }
      interaction.type = Interaction::Type::GEOMETRY;
      return true;
    }
  }
  return false;
}

Ground::Ground(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent)
    : Geometry(),
      position(std::move(position)),
      size(std::move(dimension)),
      normal(std::move(normal)),
      tangent(std::move(tangent)) {
        normal = normal.normalized();
        tangent = tangent.normalized();
      }