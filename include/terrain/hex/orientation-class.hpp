#pragma once
#include "terrain/hex/types.hpp"
namespace mundus::terrain::hex
{
  template <FloatingType ScalarType>
  struct Orientation
  {
    using Matrix = glm::mat<2, 2, ScalarType>;
    Matrix forward;
    Matrix inverse;
    ScalarType start_angle;

    Orientation(const Matrix &forward, const Matrix &inverse, ScalarType angle)
        : forward(forward), inverse(inverse), start_angle(angle)
    {
    }
    Orientation(ScalarType f0, ScalarType f1, ScalarType f2, ScalarType f3,
                ScalarType b0, ScalarType b1, ScalarType b2, ScalarType b3,
                ScalarType start_angle_)
        : forward(f0, f2, f1, f3), 
          inverse(b0, b2, b1, b3), 
          start_angle(start_angle_) {}
    static Orientation LayoutPointed() {
      return Orientation(
        {std::sqrt(3.0), std::sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0},
        {std::sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0},
        0.5
      );
    }
    static Orientation LayoutFlat()
    {
      return Orientation(
        {3.0 / 2.0, 0.0, std::sqrt(3.0) / 2.0, std::sqrt(3.0)},
        {2.0 / 3.0, 0.0, -1.0 / 3.0, std::sqrt(3.0) / 3.0},
        0.0
      );
    }
  }
}