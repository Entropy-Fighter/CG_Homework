#ifndef _BSPLINE_H_
#define _BSPLINE_H_

#include "defines.h"
#include "object.h"
#include <vector>

class BsplineCurve{
    public :
        std::vector<vec3> control_points_;
        int degree;
        std::vector<float> knots;

        BsplineCurve(int m, int d);
        BsplineCurve(std::vector<vec3> control_points, int d);      
        void generate_uniform_knots();
        void setControlPoint(int i, vec3 point);

        Vertex evaluate(std::vector<vec3> control_points, float t);
        Vertex evaluate(float t);
        int find(float t);
};

class BsplineSurface{
    public :
        std::vector<std::vector<vec3>> control_points_m_;
        std::vector<std::vector<vec3>> control_points_n_;
        std::vector<std::vector<vec3>> control_points_;

        std::vector<float> knots_m;
        std::vector<float> knots_n;

        int degree_m;
        int degree_n;
        
        BsplineSurface(int m, int n, int d_m, int d_n);    
        void generate_uniform_knots();
        void setControlPoint(int i, int j, vec3 point);
        Vertex evaluate1(float u, float v);
        Vertex evaluate2(float u, float v);
        Vertex evaluate(float u, float v);
        Object generateObject();
};

#endif
