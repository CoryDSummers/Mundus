#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "voronoi/types.h"
#include "voronoi/cell.h"
namespace voronoi{
  class Generator
  {
    public:
      using DiagramType = boost::polygon::voronoi_diagram<double>;
      CellArray operator()(const IntegerPointArray & original_seeds, int map_width, int map_height);
    private:
      void ExtractCellData(const boost::polygon::voronoi_cell<double> & cell, voronoi::Cell & terrain_cell, const int map_width, const std::size_t valid_point_count);
      void ExtractNeighbors(const boost::polygon::voronoi_cell<double> & cell, const boost::polygon::voronoi_edge<double> * edge, Cell & terrain_cell, const std::size_t valid_point_count);
      CellArray InitializeTerrainCells(const IntegerPointArray & original_seeds, const DiagramType::cell_container_type & cells, const int map_width);
      IntegerPointArray GeneratePoints(const IntegerPointArray & original_seeds, int map_width, int map_height);
    };
}