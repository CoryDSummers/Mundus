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
void DrawConnections(image::Image & image, std::size_t index, voronoi::CellArray const & cells, std::vector<int> &visited_array, std::mt19937 & gen)
{
  visited_array[index] = 1;
  std::vector<glm::vec2> float_vertices;
  std::transform(cells[index].vertices.begin(), cells[index].vertices.end(), std::back_inserter(float_vertices), [](const glm::ivec2 &v)
            { return glm::vec2(v); }); 
  //FillPolygon(image, image::RGBA(gen()), float_vertices);
  for(auto & neighbor_index : cells[index].neighbors)
  {
    if(visited_array[neighbor_index] == 0)
    {
      DrawConnections(image, neighbor_index, cells, visited_array, gen);
    }
    if(glm::distance(glm::vec2(cells[index].seed), glm::vec2(cells[neighbor_index].seed)) < image.Width() / 2.0)
      image::DrawLine(image, image::RGBA(0), cells[index].seed, cells[neighbor_index].seed);
  }
}
void SaveConnectionImage(std::string const & file_name, voronoi::CellArray const & cells, int width, int height, int seed)
{
  std::mt19937 gen(seed);
  image::Image image(width, height);
  std::vector<int> visited_array(cells.size(), 0);
  DrawConnections(image, 0, cells, visited_array, gen);
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
  const int dividator = 80; //std::stoi(argv[1]);
  const int plate_dividator = 16; //std::stoi(argv[2]);
  auto seeds = voronoi::GenerateJitteredSeeds(map_width, map_height, dividator, dividator, gen());
  voronoi::CellArray cells = voronoi::Generator()(seeds, map_width, map_height);
  terrain::tectonic::Map::Parameters terrain_parameters(map_width, map_height, 8, 7, seed);
  map.Initialize(terrain_parameters, cells);
  map.DijkstraNoiseFillGeneratePlates();
  SaveImage("voronoi.jpg", cells, map_width, map_height, gen());
  SaveTectonicImage("tectonic.jpg", map, map_width, map_height, gen());
  SaveConnectionImage("connections.jpg", cells, map_width, map_height, seed);
  return 0;
}