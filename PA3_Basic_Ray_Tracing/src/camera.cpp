#include "camera.h"

Camera::Camera()
    : position(0, -1, 0), fov(45), focal_len(1) {
  lookAt({0, 0, 0}, {0, 0, 1});
}

Ray Camera::generateRay(float dx, float dy) {
  // TODO: Your code here
  Vec3f position;
  Vec3f dir;

  float width = image->getResolution()[0];
  float height = image->getResolution()[1];
  float screen_x, screen_y;
  screen_x = (2 * (dx)/width - 1) * width/height *  tanf(fov * PI / 180 / 2) * focal_len;
  screen_y = (2 * (dy)/height - 1) *  tanf(fov * PI / 180 / 2) * focal_len;
  Vec3f screenCenter = this->position - focal_len * forward;
  position = screenCenter + screen_x * right + screen_y * up;
  dir = (position - this->position).normalized();

  return Ray{position, dir};
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
  // TODO: Your code here
  forward = (position - look_at).normalized();
  right = ref_up.cross(forward).normalized();
  up = forward.cross(right).normalized();
}

void Camera::setPosition(const Vec3f &pos) {
  position = pos;
}
Vec3f Camera::getPosition() const {
  return position;
}
void Camera::setFov(float new_fov) {
  fov = new_fov;
}
float Camera::getFov() const {
  return fov;
}

void Camera::setImage(std::shared_ptr<ImageRGB> &img) {
  image = img;
}
std::shared_ptr<ImageRGB> &Camera::getImage() {
  return image;
}

std::vector<Vec2f> Camera::sample_pixel(int dx, int dy, int sample_num){
  std::vector<Vec2f> points;
  float magic_angle = 26.6f * PI / 180;
  Eigen::Matrix2f rmat;
  rmat << cosf(magic_angle), -sinf(magic_angle), sinf(magic_angle), cosf(magic_angle);
  // rmat << sinf(magic_angle), -cosf(magic_angle), cosf(magic_angle), sinf(magic_angle);
  for(int i = 0; i < sample_num; i++){
    for(int j = 0; j < sample_num; j++){
  	  Vec2f magic_s = rmat * Vec2f(((float)(i + 0.5)/(float)sample_num - 0.5), (float)(j + 0.5)/(float)sample_num - 0.5);
  	  if(magic_s[0] >= -0.5f && magic_s[0] <= 0.5f){
        if(magic_s[1] >= -0.5f && magic_s[1] <= 0.5f){
          points.push_back(Vec2f(magic_s[0] + (float)dx + 0.5f, magic_s[1] + (float)dy + 0.5f));
        }
  	  }
    }
  }
  return points;
}

std::vector<Vec2f> Camera::sample_pixel_uniform(int dx, int dy, int sample_num){
  std::vector<Vec2f> points;
  float magic_angle = 26.6f * PI / 180;
  Eigen::Matrix2f rmat;
  rmat << cosf(magic_angle), -sinf(magic_angle), sinf(magic_angle), cosf(magic_angle);
  // rmat << sinf(magic_angle), -cosf(magic_angle), cosf(magic_angle), sinf(magic_angle);
  for(int i = 0; i < sample_num; i++){
    for(int j = 0; j < sample_num; j++){
  	  Vec2f magic_s = Vec2f(((float)(i + 0.5)/(float)sample_num - 0.5), (float)(j + 0.5)/(float)sample_num - 0.5);
  	  if(magic_s[0] >= -0.5f && magic_s[0] <= 0.5f){
        if(magic_s[1] >= -0.5f && magic_s[1] <= 0.5f){
          points.push_back(Vec2f(magic_s[0] + (float)dx + 0.5f, magic_s[1] + (float)dy + 0.5f));
        }
  	  }
    }
  }
  return points;
}

