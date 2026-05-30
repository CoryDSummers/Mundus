#pragma once
#include <array>
#include <numbers>
#include <vector>

#include <glm/glm.hpp>


namespace image::shapes
{
  
  using VertexArray = std::vector<glm::vec2>;
  inline VertexArray GetHexagon()
  {
    std::vector<glm::vec2> k_hexagon(6);
    for (int i = 0; i < 6; ++i) {
        // Pointy-topped hex angles: 30, 90, 150, 210, 270, 330 degrees
        float angle_deg = 60.0f * i + 30.0f;
        float angle_rad = std::numbers::pi_v<float> / 180.0f * angle_deg;
        
        k_hexagon[i] = glm::vec2(std::cos(angle_rad), std::sin(angle_rad));
    }
    return k_hexagon;
  }
}