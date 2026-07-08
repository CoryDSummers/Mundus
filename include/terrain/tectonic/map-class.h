#pragma once
#include <cstdint>
#include <random>
#include "terrain/tectonic/plate-class.h"
#include "voronoi/cell.h"
#include "terrain/tectonic/types.h"
namespace mundus::terrain::tectonic
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
      Map() = default;
      inline PlateArray       & GetPlates() { return plates_; }
      inline PlateIDArray     & GetPlateIDs() { return cell_plate_ids_; }
      inline GeometryArray    & GetGeometries() { return geometries_; }
      
      inline void SetPlates(PlateArray & plates) {plates_ = plate; }
      const Parameters & GetParameters() const { return parameters_; }
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