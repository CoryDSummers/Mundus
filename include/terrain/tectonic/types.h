#ifndef MUNDUS_TERRAIN_TECTONIC_TYPES_H_
#define MUNDUS_TERRAIN_TECTONIC_TYPES_H_
#include <cstdint>
#include <random>
#include "voronoi/cell.h"
namespace mundus::terrain::tectonic
{
  class Plate;
  struct Cell;
  using RandomEngine  = std::mt19937;  
  using Geometry      = voronoi::Cell;
  using GeometryArray = std::vector<Geometry>;
  using PlateArray    = std::vector<tectonic::Plate>;
  using CellArray     = std::vector<Cell>;
  using PlateID       = std::int32_t;
  using CellIndex     = std::int64_t;
  using PlateIDArray  = std::vector<PlateID>;
}
#endif // MUNDUS_TERRAIN_TECTONIC_TYPES_H_