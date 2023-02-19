#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 frag_pos[];
in vec3 normal[];

out vec3 new_pos;
out vec3 new_normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void generate_new(vec3 p, vec3 n, float constant);
void generate(vec3 p, vec3 n);

void main(){
    generate_new((frag_pos[0] + frag_pos[1] + frag_pos[2]) / 3, (normal[0] + normal[1] + normal[2]) / 3, 0.003);
    generate(frag_pos[0], normal[0]);
    generate(frag_pos[1], normal[1]);
    generate(frag_pos[2], normal[2]); 
    EndPrimitive();

}

void generate(vec3 p, vec3 n){
    new_pos = p;
    new_normal = n;
    gl_Position = projection * view * vec4(new_pos, 1.0);
    EmitVertex();
}

void generate_new(vec3 p, vec3 n, float constant){
    new_pos = n * constant + p;
    new_normal = n;
    gl_Position = projection * view * vec4(new_pos, 1.0);
    EmitVertex();
}