#pragma once
#include "terrain/hex/orientation-class.hpp"
#include "terrain/hex/types.hpp"
namespace mundus::terrain::hex
{
  template<typename OrientationScalarType, Vector2D PointType>
  class Layout{
    Orientation<OrientationScalarType> orientation;
    PointType size;
    PointType origin;
  };
}