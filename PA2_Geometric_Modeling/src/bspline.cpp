#include <bspline.h>
#include <utils.h>
#include <vector>
#include <fstream>

#include <sstream>
#include <string>

BsplineCurve::BsplineCurve(std::vector<vec3> control_points, int d){
    control_points_ = control_points;
    degree = d;
    generate_uniform_knots();
}

BsplineCurve::BsplineCurve(int m, int d){
    control_points_.resize(m);
    degree = d;
    generate_uniform_knots();
}

void BsplineCurve::setControlPoint(int i, vec3 point) {
  control_points_[i] = point;
}

void BsplineCurve::generate_uniform_knots(){
    int m = control_points_.size();
    for(int i = 0; i < degree + 1; i++){
        knots.push_back(0.0);
    } // first k + 1 knots k equal knots
    if(m - degree> 0){
        for(int i = 1; i < m - degree; i++){
            knots.push_back(float(i) / float(m - degree));
        }
    } // middle m - k - 1 internal knots
    for(int i = 0; i < degree + 1; i++){   
        knots.push_back(1.0);
    } // last k + 1 knots k equal knots
}

Vertex BsplineCurve::evaluate(std::vector<vec3> control_points, float t){
    Vertex point;
    int k = find(t);

    std::vector<vec3> points;
    for(int i = 0; i < degree + 1; i++){
        points.push_back(control_points[i + k - degree]);
    }

    for(int i = 1; i < degree + 1; i++){
        for(int j = degree; j > i - 1; j--){
            float a = 0.0f;
            if((knots[k + j - i + 1] - knots[k + j - degree]) != 0){
                a = (t - knots[k + j - degree]) / (knots[k + j - i + 1] - knots[k + j - degree]);
            }   
            points[j] = a * points[j] + (1 - a) * points[j - 1];
        }
        if(i == degree - 1){
            point.normal = glm::normalize(points[degree] - points[degree - 1]);
        }
    }
    point.position = points[degree];
    return point;
}

Vertex BsplineCurve::evaluate(float t){
    return evaluate(control_points_, t);
}

int BsplineCurve::find(float t){
    int find = -5;
    for(int i = 0; i < knots.size() - 1; i++){
        if(t >= knots[control_points_.size()]){
            return control_points_.size() - 1;
        }
        if(t <= knots[degree]){
            return degree;
        }
        if(t < knots[i + 1] && knots[i] <= t){
            return i;
        }           
    }
    return find;
}

BsplineSurface::BsplineSurface(int m, int n, int d_m, int d_n){
    degree_m = d_m;
    degree_n = d_n;
    control_points_.resize(m);
    for (auto& sub_vec : control_points_) {
        sub_vec.resize(n);
    }
    control_points_m_.resize(m);
    for (auto& sub_vec : control_points_m_) {
        sub_vec.resize(n);
    }
    control_points_n_.resize(n);
    for (auto& sub_vec : control_points_n_) {
        sub_vec.resize(m);
    }
    generate_uniform_knots();
}

void BsplineSurface::setControlPoint(int i, int j, vec3 point) {
    control_points_[i][j] = point;
    control_points_m_[i][j] = point;
    control_points_n_[j][i] = point;
}

void BsplineSurface::generate_uniform_knots(){
    int m = control_points_.size();
    int n = control_points_[0].size();
    for(int i = 0; i < degree_m + 1; i++){
        knots_m.push_back(0.0);
    } // first k + 1 knots k equal knots
    if(m - degree_m> 0){
        for(int i = 1; i < m - degree_m; i++){
            knots_m.push_back(float(i) / float(m - degree_m));
        }
    } // middle m - k - 1 internal knots
    for(int i = 0; i < degree_m + 1; i++){   
        knots_m.push_back(1.0);
    } // last k + 1 knots k equal knots

    for(int i = 0; i < degree_n + 1; i++){
        knots_n.push_back(0.0);
    } // first k + 1 knots k equal knots
    if(n - degree_n> 0){
        for(int i = 1; i < n - degree_n; i++){
            knots_n.push_back(float(i) / float(n - degree_n));
        }
    } // middle m - k - 1 internal knots
    for(int i = 0; i < degree_n + 1; i++){   
        knots_n.push_back(1.0);
    } // last k + 1 knots k equal knots
}


Vertex BsplineSurface::evaluate(float u, float v){
    BsplineCurve curve1(control_points_m_.size(), degree_m);
    curve1.knots = knots_m;
    for(int i = 0; i < control_points_m_.size(); i++){
        BsplineCurve curve_tmp(control_points_m_[i], degree_n);
        curve1.setControlPoint(i, curve_tmp.evaluate(v).position);
    }
    Vertex point = curve1.evaluate(u);

    BsplineCurve curve2(control_points_n_.size(), degree_n);
    curve2.knots = knots_n;
    for(int i = 0; i < control_points_n_.size(); i++){
        BsplineCurve curve_tmp(control_points_n_[i], degree_m);
        curve2.setControlPoint(i, curve_tmp.evaluate(u).position);
    }
    Vertex point2 = curve2.evaluate(v);
    point.normal = glm::normalize(glm::cross(point.normal, point2.normal));
    return point;
}

Vertex BsplineSurface::evaluate1(float u, float v){
    BsplineCurve curve1(control_points_m_.size(), degree_m);
    curve1.knots = knots_m;
    for(int i = 0; i < control_points_m_.size(); i++){
        BsplineCurve curve_tmp(control_points_m_[i], degree_n);
        curve1.setControlPoint(i, curve_tmp.evaluate(v).position);
    }
    return curve1.evaluate(u);
}

Vertex BsplineSurface::evaluate2(float u, float v){
    BsplineCurve final_curve(control_points_n_.size(), degree_n);
    final_curve.knots = knots_n;
    for(int i = 0; i < control_points_n_.size(); i++){
        BsplineCurve tempcurve(control_points_n_[i], degree_m);
        final_curve.setControlPoint(i, tempcurve.evaluate(v).position);
    }
    return final_curve.evaluate(u);
}

Object BsplineSurface::generateObject(){
    Object obj;
    int num = 30;
    for(int i = 0; i < num + 1; i++){
        float u = (float)i / (float)num;
        for(int j = 0; j < num + 1; j++){
            float v = (float)j / (float)num;
            obj.vertices.push_back(evaluate(u,v));
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