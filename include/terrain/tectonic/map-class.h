#pragma once
#include <cstdint>
#include <random>
#include "terrain/tectonic/plate-class.h"
#include "voronoi/cell.h"
namespace terrain::tectonic
{
  class Map
  {
    public:
      struct Parameters
      {
        std::uint32_t map_width;
        std::uint32_t map_height;
        std::uint32_t major_plate_count;
        std::uint32_t minor_plate_count; 
        std::int32_t  seed;
      };
      using Geometry      = voronoi::Cell;
      using GeometryArray = std::vector<Geometry>;
      using PlateArray    = std::vector<tectonic::Plate>;
      using PlateID       = int;
      using PlateIDArray  = std::vector<PlateID>;
      Map() = default;
      PlateArray & GetPlateArray() { return plates_; }
      PlateIDArray & GetPlateIDArray() { return cell_plate_ids_; }
      GeometryArray & GetGeometryArray() { return geometries_; }
      void Initialize(Parameters & parameters, const GeometryArray & generated_cells);
      void GeneratePlatesSimple(int map_width, int map_height, int grid_cols, int grid_rows, int seed);
      void DijkstraNoiseFillGeneratePlates();
    private:
      void WarpPlateBoundaries(int map_width, float warp_strength, float frequency, int seed);
      void InitializePlates(std::vector<PlateParameters> & generation_parameters, std::uniform_int_distribution<int> & cell_picker, std::mt19937 & rng_generator);
      int  GetSpacedStartingCell(std::uniform_int_distribution<int> &, const float min_distance, std::mt19937 & gen);
      Parameters    parameters_;
      GeometryArray geometries_;
      PlateIDArray  cell_plate_ids_;
      PlateArray    plates_;
      
  };

}