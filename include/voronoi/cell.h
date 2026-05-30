#pragma once
#include "voronoi/types.h"

namespace voronoi
{
  struct Cell{
    Point seed;
    VertexArray vertices;
    std::vector<int> neighbors;
  };
}