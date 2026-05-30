#pragma once
#include <algorithm>
#include <concepts>
#include <type_traits>


#include <cmath>

#include <glm/vec2.hpp>
namespace terrain::math
{
  template<typename T>
    concept Vector2D = requires(T v) {
        v.x;
        v.y;
    };
  template<
    Vector2D VectorType,
    typename ScalarType = std::decay_t<decltype(std::declval<Vector2D>().x)>
  >
  inline float GetCylindricalDistance(const VectorType & p1, const VectorType & p2, ScalarType map_width)
  {
    float x1 = static_cast<float>(p1.x);
    float x2 = static_cast<float>(p2.x);
    float y1 = static_cast<float>(p1.y);
    float y2 = static_cast<float>(p2.y);
    float width = static_cast<float>(map_width);

    float dx = std::abs(x1 - x2);
    float dy = std::abs(y1 - y2);
    if(dx > width * .5f)
    {
      dx = width - dx;
    }
    
    return std::sqrt(dx * dx + dy * dy);
  }
}