#pragma once
#include <vector>

#include "voronoi/cell.h"
namespace mundus::terrain::tectonic
{
  struct Cell
  {
    using GeometryType = voronoi::Cell;
    GeometryType geometry;
    Cell() = default;
    Cell(GeometryType const & geometry)
      : geometry(geometry)
      {}
  };
}