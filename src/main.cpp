#include <iostream>
#include <random>
#include <string>
#include "terrain/hex/grid-class.hpp"
#include "image/image.h"
#include "image/drawing.h"

void SaveHexGridToJPEG(std::string const & file_name, const mundus::terrain::hex::HexGrid<int, glm::vec3> & grid)
{
  
}

int main(int argc, char * argv[])
{
  mundus::terrain::hex::HexGrid<int, glm::vec3> grid(128, 80, 0);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 1);
  auto * grid_data = grid.data();
  for(std::size_t y = 0; y < grid.GetHeight(); ++y)
  {
    for(std::size_t x = 0; x < grid.GetWidth(); ++x)
    {
      grid_data[y * grid.GetWidth() + x] = distrib(gen);
    }
  }
  return 0;
}