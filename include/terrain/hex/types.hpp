#pragma once
#include <glm/vec3.hpp>
#include <concepts>
namespace mundus::terrain::hex
{
  template <typename T>
  concept Vector2D = requires(T v) {
    typename T::value_type;
    v.x;
    v.y;
  };
  template <typename T>
  concept Vector3D = requires(T v) {
    typename T::value_type;
    v.x;
    v.y;
    v.z;
  };
  template< class T >
  concept FloatingType = std::is_floating_point_v<T>;
}