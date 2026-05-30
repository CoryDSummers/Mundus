#pragma once
#include <vector>

#include "voronoi/cell.h"
namespace terrain::tectonic
{
  struct Cell;
  using  CellList = std::vector<Cell>;
  struct Cell
  {
    using IDType = int;
    using GeometryType = voronoi::Cell;
    GeometryType geometry;
    Cell() = default;
    Cell(GeometryType const & geometry)
      : geometry(geometry)
      {}
  };
}