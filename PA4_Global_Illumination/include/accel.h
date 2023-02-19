#ifndef ACCEL_H_
#define ACCEL_H_

#include "core.h"
#include "ray.h"

struct AABB {
  // the minimum and maximum coordinate for the AABB
  Vec3f low_bnd;
  Vec3f upper_bnd;
  /// test intersection with given ray.
  /// ray distance of entrance and exit point are recorded in t_in and t_out
  AABB() : low_bnd(99999, 99999, 99999), upper_bnd(-99999, -99999, -99999) {}
  AABB(Vec3f low, Vec3f upper) : low_bnd(low), upper_bnd(upper) {}
  /// construct an AABB from three vertices of a triangle.
  AABB(const Vec3f &v1, const Vec3f &v2, const Vec3f &v3);
  /// Construct AABB by merging two AABBs
  AABB(const AABB &a, const AABB &b);
  bool intersect(const Ray &ray, float *t_in, float *t_out);
  /// Get the AABB center
  [[nodiscard]] Vec3f getCenter() const { return (low_bnd + upper_bnd) / 2; }
  /// Get the length of a specified side on the AABB
  [[nodiscard]] float getDist(int dim) const { return upper_bnd[dim] - low_bnd[dim]; }
  /// Check whether the AABB is overlapping with another AABB
  [[nodiscard]] bool isOverlap(const AABB &other) const;
};

struct BVHNode {
  BVHNode *left;
  BVHNode *right;
  // bounding box of current node.
  AABB aabb;
  // index of triangles in current BVH leaf node.
  std::vector<int> triangles;
  int i = 0;
  int size = 0;
};

struct Triangle{
  Triangle() = default;
  Triangle(Vec3f p_0, Vec3f p_1, Vec3f p_2, Vec3f n_0, Vec3f n_1, Vec3f n_2) : p0(p_0), p1(p_1), p2(p_2), n0(n_0), n1(n_1), n2(n_2){
    bary = (p0 + p1 + p2) / 3;
  }
  Vec3f p0;
  Vec3f p1;
  Vec3f p2;
  Vec3f n0;
  Vec3f n1;
  Vec3f n2;
  Vec3f bary;
};
// You may need to add your code for BVH construction here.

#endif //ACCEL_H_