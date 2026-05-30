#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "voronoi/types.h"
#include "voronoi/cell.h"
namespace voronoi{
  class Generator
  {
    public:
    CellArray operator()(const PointArray & original_seeds, int map_width, int map_height);
  };
}