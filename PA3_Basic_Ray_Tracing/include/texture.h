#ifndef CS171_HW3_INCLUDE_TEXTURE_H_
#define CS171_HW3_INCLUDE_TEXTURE_H_

#include <vector>
#include <string>

#include "core.h"

// class Texture {
//  public:
//   enum Type { DIFF, NORM, DISP };
//   Type type;
//   void loadTextureFromFile(const std::string &path);

//  private:
//   std::vector<uint8_t> tex_data;
// };
class Texture {
 public:
    enum Type { NONE, DIFF, NORM, DISP, GRID };
    Type type;
    Texture();
    void loadTextureFromFile(const std::string &path);
    Vec3f get_tex_data(int index);
    Vec3f bilinear_interpolation(float u, float v);
    Vec3f basic_interpolation(float u, float v);
    int weight;
    int height;
    int channel;
 private:
    std::vector<uint8_t> tex_data;
};

#endif //CS171_HW3_INCLUDE_TEXTURE_H_
