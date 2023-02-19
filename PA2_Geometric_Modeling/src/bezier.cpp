#include <bezier.h>
#include <utils.h>
#include <vector>
#include <fstream>

#include <sstream>
#include <string>

BezierCurve::BezierCurve(int m) { control_points_.resize(m); }

BezierCurve::BezierCurve(std::vector<vec3>& control_points) {
  control_points_ = control_points;
}

void BezierCurve::setControlPoint(int i, vec3 point) {
  control_points_[i] = point;
}

/**
 * TODO: evaluate the point at t with the given control_points
 */
Vertex BezierCurve::evaluate(std::vector<vec3>& control_points, float t) {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  // UNIMPLEMENTED;
  Vertex point;
  std::vector<vec3> points = control_points;
  for(int i = 1; i < points.size(); i++){   
    for(int j = 0; j < points.size() - i; j++){
      if(i == points.size() - 1)
        point.normal = glm::normalize(points[1] - points[0]);
      points[j] = (1 - t) * points[j] + t * points[j + 1];
    }
  } 
  
  point.position = points[0];
  return point;
}

Vertex BezierCurve::evaluate(float t) {
  return evaluate(control_points_, t);
}

/**
 * TODO: generate an Object of the current Bezier curve
 */
Object BezierCurve::generateObject() {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  // UNIMPLEMENTED;
  Object obj;
  int num = 30;
  for(int i = 0; i <= num; i++){
    float t = (float)i / (float)num;
    obj.vertices.push_back(evaluate(t));
    if(i + 1 <= num){
      obj.indices.push_back(i);
      obj.indices.push_back(i + 1);
    } 
  }
  obj.draw_mode.primitive_mode = GL_LINES;
  return obj;
}

BezierSurface::BezierSurface(int m, int n) {
  control_points_m_.resize(m);
  for (auto& sub_vec : control_points_m_) {
    sub_vec.resize(n);
  }
  control_points_n_.resize(n);
  for (auto& sub_vec : control_points_n_) {
    sub_vec.resize(m);
  }
}

/**
 * @param[in] i: index (i < m)
 * @param[in] j: index (j < n)
 * @param[in] point: the control point with index i, j
 */
void BezierSurface::setControlPoint(int i, int j, vec3 point) {
  control_points_m_[i][j] = point;
  control_points_n_[j][i] = point;
}

/**
 * TODO: evaluate the point at (u, v) with the given control points
 */
Vertex BezierSurface::evaluate(std::vector<std::vector<vec3>>& control_points,
                               float u, float v) {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  // UNIMPLEMENTED;
  BezierCurve curve1(control_points.size());
  for(int i = 0; i < control_points.size(); i++){
    BezierCurve curve_tmp(control_points[i]);
    curve1.setControlPoint(i, curve_tmp.evaluate(v).position);
  }
  Vertex point = curve1.evaluate(u);

  BezierCurve curve2(control_points[0].size());
  for(int i = 0; i < control_points[0].size(); i++){   
    std::vector<vec3> points_tmp;
    for(int j = 0; j < control_points.size(); j++){
      points_tmp.push_back(control_points[j][i]);
    }
    BezierCurve curve_tmp(points_tmp);
    curve2.setControlPoint(i, curve_tmp.evaluate(u).position);
  }
  Vertex point2 = curve2.evaluate(v);
  point.normal = glm::normalize(glm::cross(point.normal, point2.normal));
  return point;
}

/**
 * TODO: generate an Object of the current Bezier surface
 */
Object BezierSurface::generateObject() {
  // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
  // UNIMPLEMENTED;
  int num = 30;
  Object obj;
  for(int i = 0; i < num + 1; i++){
    float u = (float)i / (float)num;
    for(int j = 0; j < num + 1; j++){
      float v = (float)j / (float)num;
      obj.vertices.push_back(evaluate(control_points_m_, u, v));
    }
  }
  
  for(int i = 0; i < num; i++){
    for(int j = 0; j < num; j++){
      obj.indices.push_back(i * (num + 1) + j + 1);
      obj.indices.push_back((i + 1) * (num + 1) + j);
      obj.indices.push_back(i * (num + 1) + j);

      obj.indices.push_back(i * (num + 1) + j + 1);
      obj.indices.push_back((i + 1) * (num + 1) + j);
      obj.indices.push_back((i + 1) * (num + 1) + j + 1);
    }
  }
  obj.draw_mode.primitive_mode = GL_TRIANGLES;
  return obj;
}

/**
 * TODO: read in bzs file to generate a vector of Bezier surface
 * for the first line we have b p m n
 * b means b bezier surfaces, p means p control points.
 *
 */
std::vector<BezierSurface> read(const std::string &path) {
  //UNIMPLEMENTED;
  std::vector<BezierSurface> surfaces;
  std::ifstream bzsFile;
  std::string eachLine;
  bzsFile.open(path);
  if(!bzsFile.is_open()){
    std::cout << "OBJECTFILE__READ_FAILURE" << std::endl;
    return surfaces;
  }

  std::getline(bzsFile, eachLine);
  std::stringstream lineSream(eachLine);
  int b;
  int p;
  int m;
  int n;
  lineSream >> b >> p >> m >> n;
  // std::cout << b << " " << p << " " << m << " " << n;

  for(int i = 0; i < b; i++){
    BezierSurface surface(m, n);
    std::getline(bzsFile, eachLine);
    std::stringstream lineStream(eachLine);
    for(int j = 0; j < m*n; j++){
      int index = 0;
      lineStream >> index;
      // if(i == 0 && j == 1)
      //   std::cout << index <<std::endl;
      surface.inds.push_back(index);
    }
    surfaces.push_back(surface);
  }

  std::vector<vec3> control_points;
  for(int i = 0; i < p; i++){
    vec3 tmp = vec3(0.0f, 0.0f, 0.0f);
    std::getline(bzsFile, eachLine);
    std::stringstream lineStream(eachLine);
    lineStream >> tmp.x >> tmp.y >> tmp.z;
    control_points.push_back(tmp);
  }
  
  for(int i = 0; i < b; i++){
    for(int j = 0; j < m; j++){
      for(int k = 0; k < n; k++){
        surfaces[i].setControlPoint(j, k, control_points[surfaces[i].inds[j * n + k]]);
      }
    }
  }

  return surfaces;
}