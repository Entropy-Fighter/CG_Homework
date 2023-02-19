#include "geometry.h"

#include <utility>
#include <iostream>

TriangleMesh::TriangleMesh(std::vector<Vec3f> vertices, std::vector<Vec3f> normals,
                           std::vector<int> v_index, std::vector<int> n_index) :
    vertices(std::move(vertices)),
    normals(std::move(normals)),
    v_indices(std::move(v_index)),
    n_indices(std::move(n_index)),
    bvh(nullptr) {}

bool TriangleMesh::intersect(Ray &ray, Interaction &interaction) const {
  if (bvh != nullptr) {
    bvhHit(bvh, interaction, ray);
  } else {
    // If you did not implement BVH
    // directly loop through all triangles in the mesh and test intersection for each triangle.
    for (int i = 0; i < v_indices.size() / 3; i++) {
      Vec3i v_idx(v_indices[3 * i], v_indices[3 * i + 1], v_indices[3 * i + 2]);
      Vec3i n_idx(n_indices[3 * i], n_indices[3 * i + 1], n_indices[3 * i + 2]);
      Interaction temp;
      if (intersectOneTriangle(ray, temp, v_idx, n_idx) && (temp.dist < interaction.dist)) {
        interaction = temp;
      }
    }
  }
  return interaction.type != Interaction::Type::NONE;
}

void TriangleMesh::setMaterial(std::shared_ptr<BSDF> &new_bsdf) {
  bsdf = new_bsdf;
}
void TriangleMesh::buildBVH() {
  // TODO: your implementation
  for(int i = 0; i < v_indices.size() / 3; i++){
    Vec3i v_idx(v_indices[3 * i], v_indices[3 * i + 1], v_indices[3 * i + 2]);
    Vec3i n_idx(n_indices[3 * i], n_indices[3 * i + 1], n_indices[3 * i + 2]);
    Triangle tri(vertices[v_idx[0]], vertices[v_idx[1]], vertices[v_idx[2]], normals[n_idx[0]], normals[n_idx[1]], normals[n_idx[2]]); 
    triangles.push_back(tri);
  }
  bvh = SAH(0, triangles.size() - 1);
}

bool TriangleMesh::intersectOneTriangle(Ray &ray,
                                        Interaction &interaction,
                                        const Vec3i &v_idx,
                                        const Vec3i &n_idx) const {
  Vec3f v0 = vertices[v_idx[0]];
  Vec3f v1 = vertices[v_idx[1]];
  Vec3f v2 = vertices[v_idx[2]];
  Vec3f v0v1 = v1 - v0;
  Vec3f v0v2 = v2 - v0;
  Vec3f pvec = ray.direction.cross(v0v2);
  float det = v0v1.dot(pvec);

  float invDet = 1.0f / det;

  Vec3f tvec = ray.origin - v0;
  float u = tvec.dot(pvec) * invDet;
  if (u < 0 || u > 1) return false;
  Vec3f qvec = tvec.cross(v0v1);
  float v = ray.direction.dot(qvec) * invDet;
  if (v < 0 || u + v > 1) return false;
  float t = v0v2.dot(qvec) * invDet;
  if (t < ray.t_min || t > ray.t_max) return false;

  interaction.dist = t;
  interaction.pos = ray(t);
  interaction.normal = (u * normals[n_idx[1]] + v * normals[n_idx[2]]
      + (1 - u - v) * normals[n_idx[0]]).normalized();
  interaction.material = bsdf;
  interaction.type = Interaction::Type::GEOMETRY;
  return true;
}

void TriangleMesh::bvhHit(BVHNode *p, Interaction &interaction,
                          Ray &ray) const {
  // TODO: traverse through the bvh and do intersection test efficiently.
  if(!p){
    return;
  }

  if(p->size > 0){
    for(int i = p->i; i < p->i + p->size; i++){
      Interaction tmp;
      if(intersectOneTriangle(ray, tmp, triangles[i]) && (tmp.dist < interaction.dist)){
        interaction = tmp;
      }
    }
  }

  float in_l, out_l, in_r, out_r;
  bool l = 0;
  bool r = 0;

  if(p->left != nullptr){
    l = p->left->aabb.intersect(ray, &in_l, &out_l);
  }

  if(l){
    bvhHit(p->left, interaction, ray);
  }

  if(p->right != nullptr){
    r = p->right->aabb.intersect(ray, &in_r, &out_r);
  }

  if(r){
    bvhHit(p->right, interaction, ray);
  }
}

bool TriangleMesh::intersectOneTriangle(Ray &ray, Interaction &interaction, const Triangle &triangle) const{
  Vec3f v0 = triangle.p0;
  Vec3f v1 = triangle.p1;
  Vec3f v2 = triangle.p2;
  Vec3f v0v1 = v1 - v0;
  Vec3f v0v2 = v2 - v0;
  Vec3f pvec = ray.direction.cross(v0v2);
  float det = v0v1.dot(pvec);

  float invDet = 1.0f / det;

  Vec3f tvec = ray.origin - v0;
  float u = tvec.dot(pvec) * invDet;
  if(u < 0 || u > 1)
    return false;
  Vec3f qvec = tvec.cross(v0v1);
  float v = ray.direction.dot(qvec) * invDet;
  if(v < 0 || u + v > 1)
    return false;
  float t = v0v2.dot(qvec) * invDet;
  if(t < ray.t_min || t > ray.t_max)
    return false;

  interaction.dist = t;
  interaction.pos = ray(t);
  interaction.normal = (u * triangle.n1 + v * triangle.n2 + (1 - u - v) * triangle.n0).normalized();
  interaction.material = bsdf;
  interaction.type = Interaction::Type::GEOMETRY;
  return true;
}

bool x(const Triangle& a, const Triangle& b){
  return a.bary.x() < b.bary.x();
}
bool y(const Triangle& a, const Triangle& b){
  return a.bary.y() < b.bary.y();
}
bool z(const Triangle& a, const Triangle& b){
  return a.bary.z() < b.bary.z();
}

BVHNode* TriangleMesh::SAH(int l, int r){
  if(l > r)
    return nullptr;

  AABB tmp;
  BVHNode *node = new BVHNode();
  node->aabb = AABB();

  for(int i = l; i <= r; i++){
    tmp = AABB(triangles[i].p0, triangles[i].p1, triangles[i].p2);
    node->aabb = AABB(node->aabb, tmp);
  }

  if((r - l + 1) <= 4){
    node->size = r - l + 1;
    node->i = l;
    return node;
  }

  float cost = 99999;
  int s = l + (r - l) / 2;
  int d = 0;

  for(int d_ = 0; d_ < 3; d_++){
    if(d_ == 0){
      std::sort(&triangles[0] + l, &triangles[0] + r + 1, x);
    }else if(d_ == 1){
      std::sort(&triangles[0] + l, &triangles[0] + r + 1, y);
    }else{
      std::sort(&triangles[0] + l, &triangles[0] + r + 1, z);
    }
    
    std::vector<AABB> aabb_l(r - l + 1, AABB());
    aabb_l[0] = AABB(triangles[l].p0, triangles[l].p1, triangles[l].p2);
    for(int i = l + 1; i <= r; i++){
      aabb_l[i - l] = AABB(AABB(triangles[i].p0, triangles[i].p1, triangles[i].p2), aabb_l[i - l - 1]);
    }

    std::vector<AABB> aabb_r(r - l + 1, AABB());
    aabb_r[r - l] = AABB(triangles[r].p0, triangles[r].p1, triangles[r].p2);
    for(int i = r - 1; i >= l; i--){
      aabb_r[i - l] = AABB(AABB(triangles[i].p0, triangles[i].p1, triangles[i].p2), aabb_r[i - l + 1]);
    }

    float cost_ = 99999;
    int s_ = l;
    for (int i = l; i <= r - 1; i++){   
      tmp = aabb_l[i - l];
      float left_cost = (i - l + 1) * 2 * (tmp.getDist(0) * tmp.getDist(1) + tmp.getDist(0) * tmp.getDist(2) + tmp.getDist(1) * tmp.getDist(2));

      tmp = aabb_r[i + 1 - l];
      float right_cost = (r - i) * 2 * (tmp.getDist(0) * tmp.getDist(1) + tmp.getDist(0) * tmp.getDist(2) + tmp.getDist(1) * tmp.getDist(2));

      if(left_cost + right_cost < cost_){
        s_ = i;
        cost_ = left_cost + right_cost;  
      }
    }

    if(cost_ < cost){
      d = d_;
      s = s_;
      cost = cost_;   
    }
  }

  if(d == 0){
    std::sort(&triangles[0] + l, &triangles[0] + r + 1, x);
  }else if(d == 1){
    std::sort(&triangles[0] + l, &triangles[0] + r + 1, y);
  }else{
    std::sort(&triangles[0] + l, &triangles[0] + r + 1, z);
  }
  
  node->left = SAH(l, s);
  
  node->right = SAH(s + 1, r);

  return node;
}