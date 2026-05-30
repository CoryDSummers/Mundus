#pragma once
#include "terrain/tectonic/plate-class.h"
#include "voronoi/cell.h"
namespace terrain::tectonic
{
  class Map
  {
    public:
      using Geometry      = voronoi::Cell;
      using GeometryArray = std::vector<Geometry>;
      using PlateArray    = std::vector<tectonic::Plate>;
      using PlateID       = int;
      using PlateIDArray  = std::vector<PlateID>;
      Map() = default;
      PlateArray & GetPlateArray() { return plates_; }
      PlateIDArray & GetPlateIDArray() { return cell_plate_ids_; }
      GeometryArray & GetGeometryArray() { return geometries_; }
      void Initialize(const GeometryArray & generated_cells);
      void GeneratePlatesSimple(int map_width, int map_height, int grid_cols, int grid_rows, int seed);
      void DijkstraNoiseFillGeneratePlates(int map_width, int map_height, int plate_count, int seed);
    private:
      void WarpPlateBoundaries(int map_width, float warp_strength, float frequency, int seed);
      GeometryArray geometries_;
      PlateIDArray  cell_plate_ids_;
      PlateArray    plates_;
      
  };

}