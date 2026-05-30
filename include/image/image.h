#pragma once
#include <vector>
#include <string>
#include "image/rgb.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image/stb_image_write.h"
namespace image{
  class Image
  {
    public:
    Image() = default;
    Image(unsigned width, unsigned height, RGBA rgb = {0xFFFFFFFF}) 
      : width_(width)
      , height_(height)
      , image_data_(width * height, rgb)
    {

    }
    //Returns Y row
    RGBA * operator[](int index) { return image_data_.data() + (index * width_); }
    inline int Save(std::string const & file_path)
    {
      return stbi_write_jpg(file_path.c_str(), width_, height_, 4, (char *)image_data_.data(), 100);
    }
    const int Width() const { return width_; }
    const int Height() const { return height_; }
    private:
    int width_ = 1;
    int height_ = 1;
    std::vector<RGBA> image_data_;
  };
}