#pragma once
#include <random>
#include <vector>
#include <cstdint>
#include "voronoi/types.h"
namespace voronoi
{
  inline PointArray GenerateJitteredSeeds(int map_width, int map_height, int grid_columns, int grid_rows, std::uint32_t seed)
  {

    std::mt19937 gen(seed);
    PointArray seeds;
    seeds.reserve(grid_columns * grid_rows);

    // Calculate the width and height of each grid cell
    int cell_width  = map_width / grid_columns;
    int cell_height = map_height / grid_rows;

    // We want to jitter the point within the cell.
    // To prevent seeds from crossing into neighboring cells, the max jitter
    // is half the cell size in any direction.
    std::uniform_int_distribution<> jitter_x(-cell_width / 2 + 1, cell_width / 2 - 1);
    std::uniform_int_distribution<> jitter_y(-cell_height / 2 + 1, cell_height / 2 - 1);

    for (int row = 0; row < grid_rows; ++row)
    {
      for (int col = 0; col < grid_columns; ++col)
      {

        // 1. Find the exact center of this grid cell
        int center_x = (col * cell_width) + (cell_width / 2);
        int center_y = (row * cell_height) + (cell_height / 2);

        // 2. Apply random jitter
        int final_x = center_x + jitter_x(gen);
        int final_y = center_y + jitter_y(gen);

        seeds.push_back(Point(final_x, final_y));
      }
    }
    return seeds;
  }
}