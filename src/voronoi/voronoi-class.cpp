#include "voronoi/voronoi-class.h"
#include <cmath>
constexpr std::size_t k_extra_cap = 500;
voronoi::PointArray GeneratePoints(const voronoi::PointArray & original_seeds, int map_width, int map_height)
{
  voronoi::PointArray all_points;
  all_points.reserve(original_seeds.size() * 3 + 500);
  for(const auto & seed : original_seeds)
  {
    all_points.push_back(voronoi::Point(seed.x, seed.y));
    all_points.push_back(voronoi::Point(seed.x - map_width, seed.y));
    all_points.push_back(voronoi::Point(seed.x + map_width, seed.y));
  }
  std::size_t valid_point_count = all_points.size();
  int cap_spacing = map_width / 50;
  for (int x = -map_width; x <= map_width * 2; x += cap_spacing) {
      all_points.push_back(voronoi::Point(x, -200));              // Top Cap
      all_points.push_back(voronoi::Point(x, map_height + 200));  // Bottom Cap
  }
  return all_points;
}

template<typename EdgeType>
void ExtractNeighbors(const boost::polygon::voronoi_cell<double> & cell, EdgeType * edge, voronoi::Cell & terrain_cell, const std::size_t valid_point_count)
{
  const auto* twin = edge->twin();
  if (twin != nullptr) {
    const auto* neighbor_cell = twin->cell();
    
    // Ensure the neighbor isn't one of your top/bottom bounding caps
    if (neighbor_cell->source_index() < valid_point_count) {
      
      // Divide by 3 to map the ghost/wrapped point back to the original seed ID
      std::size_t neighbor_id = neighbor_cell->source_index() / 3;
      
      // Prevent self-referencing and duplicate neighbors
      if (neighbor_id != (cell.source_index() / 3)) {
        if (std::find(terrain_cell.neighbors.begin(), terrain_cell.neighbors.end(), neighbor_id) == terrain_cell.neighbors.end()) {
          terrain_cell.neighbors.push_back(static_cast<int>(neighbor_id));
        }
      }
    }
  }
}
void ExtractCellData(const boost::polygon::voronoi_cell<double> & cell, voronoi::Cell & terrain_cell, const int map_width, const std::size_t valid_point_count)
{
  const int k_half_width = map_width / 2;
  const auto * edge = cell.incident_edge();
  terrain_cell.neighbors.reserve(8);
  if(edge == nullptr)
    return;

  do {
    if(edge->is_primary() && edge->is_finite())
    {
      voronoi::Vertex vertex(
        static_cast<int>(std::round(edge->vertex0()->x())),
        static_cast<int>(std::round(edge->vertex0()->y()))
      );
      if(vertex.x - terrain_cell.seed.x > k_half_width)
        vertex.x -= map_width;
      else if(terrain_cell.seed.x - vertex.x > k_half_width)
        vertex.x += map_width;
      terrain_cell.vertices.push_back(vertex);
    }
    ExtractNeighbors(cell, edge, terrain_cell, valid_point_count);
    edge = edge->next();
  } while(edge != cell.incident_edge());

}
voronoi::CellArray voronoi::Generator::operator()(const PointArray & original_seeds, int map_width, int map_height)
{
  const int k_half_width = map_width / 2;
  const std::size_t k_valid_point_count = original_seeds.size() * 3;
  boost::polygon::voronoi_diagram<double> vd;
  PointArray all_points = GeneratePoints(original_seeds, map_width, map_height);
  boost::polygon::construct_voronoi(all_points.begin(), all_points.end(), &vd);

  
  CellArray  terrain_cells(original_seeds.size());

  for(std::size_t i = 0; i < vd.cells().size(); ++i)
  {
    const auto & cell = vd.cells()[i];
    if(cell.source_index() >= k_valid_point_count || cell.source_index() % 3 != 0)
      continue;
    std::size_t index = cell.source_index() / 3;
    Cell terrain_cell = { original_seeds[index], VertexArray(), std::vector<int>()};
    ExtractCellData(cell, terrain_cell, map_width, k_valid_point_count);
    terrain_cells[index] = terrain_cell;
  }
  for (std::size_t i = 0; i < terrain_cells.size(); ++i) 
  {
    for (int neighbor_id : terrain_cells[i].neighbors) 
    {
        auto & target_neighbors = terrain_cells[neighbor_id].neighbors;
        
        // If our neighbor doesn't know about us, add ourselves to their list
        if (std::find(target_neighbors.begin(), target_neighbors.end(), static_cast<int>(i)) == target_neighbors.end()) 
        {
            target_neighbors.push_back(static_cast<int>(i));
        }
    }
  }
  return terrain_cells;
}