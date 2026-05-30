#pragma once
#include <string>

#include "terrain/hex/grid-class.hpp"
#include "image/image.h"
#include "image/drawing.h"
#include "image/shapes.h"

inline void SaveHexGridToJPEG(std::string const & file_name, mundus::terrain::hex::HexGrid<int, glm::vec3> & grid, float in_radius = 5.0f)
{
  const std::size_t width = grid.GetWidth();
  const std::size_t height = grid.GetHeight();
  static const image::RGBA k_colors[2] = {0x0000FF00, 0x00FF0000};
  const image::VertexArray k_base_hexagon = image::shapes::GetHexagon();
  const float circumradius = 2.0f * in_radius / std::sqrt(3.0f);
  image::Image image(int(in_radius * 2.f * width) + in_radius, int(1.5f * circumradius * height) + circumradius);
  //Starting Position
  auto * buffer = grid.data();
  image::VertexArray vertices;
  for(int y = 0; y < height; ++y)
  {
    glm::vec2 translation{in_radius + (y & 1) * in_radius, circumradius + (1.5f * circumradius) * y};
    for(int x = 0; x < width; ++x)
    {
      vertices = k_base_hexagon;
      image::Transform(vertices, translation, {circumradius, circumradius});
      auto & tile = buffer[y * width + x];
      image::FillPolygon(image, k_colors[tile], vertices);
      translation.x += in_radius * 2.f;
    }

  }
  image.Save(file_name);
} 
