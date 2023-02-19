#version 330 core
out vec4 frag_color;

in vec3 normal;
in vec3 frag_pos;

uniform vec3 light_pos;
uniform vec3 view_pos;
uniform vec3 light_color;
uniform vec3 object_color;

uniform float light_attenuation;

void main(){
    
    float distance_to_light = length(light_pos - frag_pos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance_to_light + light_attenuation * pow(distance_to_light, 2));
    
   
    // ambient light
    float ambient_factor = 0.1;
    vec3 ambient = ambient_factor * light_color;

    // diffuse light
    vec3 normalized_normal = normalize(normal);
    vec3 light_direction = normalize(light_pos - frag_pos);
    float diff;
    if(dot(normalized_normal, light_direction) < 0){
        diff = -dot(normalized_normal, light_direction);
    }else{
        diff = dot(normalized_normal, light_direction);
    }
    // float diff = max(dot(normalized_normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

    // specular light
    float specular_strength = 0.5;
    vec3 view_direction = normalize(view_pos - frag_pos);
    vec3 reflect_direction = reflect(-light_direction, normalized_normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;


    vec3 phong_light = attenuation * (ambient + (diffuse + specular)) * object_color;
    frag_color = vec4(phong_light, 1.0);
}