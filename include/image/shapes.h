#pragma once
#include <array>
#include <vector>
#include <glm/glm.hpp>


namespace image 
{
  
  using VertexArray = std::vector<glm::vec2>;
  template<typename T>
  VertexArray GetHexagon()
  {
    static const std::array<glm::vec2, 6> k_hexagon = 
  }
  namespace internal
  {
    constexpr std::array<glm::vec2, 6> GenerateHexagon()
    {
      
    }
  }
}