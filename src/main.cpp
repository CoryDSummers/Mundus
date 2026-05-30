#include <iostream>
#include <cstdint>
#include <numeric>
#include <fstream>
#include <random>
#include <string>
#include "voronoi/voronoi-class.h"
#include "voronoi/json-serialization.h"
#include "voronoi/jittered-cell.h"
#include "image/image.h"
#include "image/drawing.h"
#include "terrain/tectonic/map-class.h"

void SaveImage(std::string const &file_name, voronoi::CellArray const &cells, int map_width, int map_height, int seed)
{
  std::mt19937 gen(seed);
  std::ofstream json_output("voronoi_cells.json");
  voronoi::JsonSerialize(cells, json_output);
  image::Image image(map_width, map_height);
  try
  {
    for (auto &cell : cells)
    {
      image::RGBA color(gen());
      std::vector<glm::vec2> float_vertices;
      std::transform(cell.vertices.begin(), cell.vertices.end(), std::back_inserter(float_vertices), [](const glm::ivec2 &v)
                     { return glm::vec2(v); });
      FillPolygon(image, color, float_vertices);
      DrawCross(image, image::RGBA(0x00FFFFFF & ~(color.value)), cell.seed, 3);
    }
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }
  image.Save(file_name);
}
void SaveTectonicImage(std::string const & file_name, terrain::tectonic::Map &map, int map_width, int map_height, int seed)
{
  const std::size_t k_cell_count = map.GetGeometryArray().size();
  std::mt19937 gen(seed);
  image::Image image(map_width, map_height);

  std::vector<image::RGBA> plate_colors(map.GetPlateArray().size());
  std::generate(plate_colors.begin(), plate_colors.end(), [&]() { return image::RGBA(gen()); });
  for(std::size_t i = 0; i < k_cell_count; ++i)
  {
    auto & cell = map.GetGeometryArray()[i];
    int plate_id = map.GetPlateIDArray()[i];
    std::vector<glm::vec2> float_vertices;
    std::transform(cell.vertices.begin(), cell.vertices.end(), std::back_inserter(float_vertices), [](const glm::ivec2 &v)
                { return glm::vec2(v); }); 
    if(plate_id == -1)
      continue;
    FillPolygon(image, plate_colors[plate_id], float_vertices);
  }
  for(auto & plate : map.GetPlateArray())
  {
    DrawCross(image, image::RGBA(0xFFFFFFFF), plate.center_seed, 8);
  }
  image.Save(file_name);
}
int main(int argc, char **argv)
{
  int map_width = 6000;
  int map_height = 3000;
  std::random_device rd;
  int seed = rd();
  std::mt19937 gen(seed);
  terrain::tectonic::Map map;
  const int dividator = 100; //std::stoi(argv[1]);
  const int plate_dividator = 16; //std::stoi(argv[2]);
  auto seeds = voronoi::GenerateJitteredSeeds(map_width, map_height, dividator, dividator, gen());
  voronoi::CellArray cells = voronoi::Generator()(seeds, map_width, map_height);
  map.Initialize(cells);
  map.DijkstraNoiseFillGeneratePlates(map_width, map_height, plate_dividator, gen());
  SaveImage("voronoi.jpg", cells, map_width, map_height, gen());
  SaveTectonicImage("tectonic.jpg", map, map_width, map_height, gen());
  return 0;
}