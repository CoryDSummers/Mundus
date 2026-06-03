#include "voronoi/json-serialization.h"
#include <string>
#include <string_view>
void PrintVertex(voronoi::Vertex vertex, std::ostream & stream, int depth = 0)
{
  stream << std::string(depth, '\t') << "{ \"x\": " << vertex.x << ", \"y\": " << vertex.y << "}";
}
void PrintNeighbors(const std::vector<int> & neighbors, std::ostream & stream)
{
  int idx = 0;
  stream << "\"neighbors\": [";

  for(; idx < neighbors.size() - 1; ++idx)
  {
    stream << neighbors[idx] << ", ";
  }
  stream << neighbors[idx] << " ]"; 
}
void PrintCell(voronoi::Cell const & cell, std::ostream & stream, int depth = 0)
{
  int i = 0;
  stream << "{\n" << std::string(depth + 1, '\t') << "\"seed\": "
    << "{ \"x\": " << cell.seed.x << ", \"y\": " << cell.seed.y << "}";
  stream <<",\n" << std::string(depth + 1, '\t');
  PrintNeighbors(cell.neighbors, stream);
  stream << ",\n" << std::string(depth + 1, '\t') << "\"vertices\": [\n";
  for(; i < cell.vertices.size() - 1; ++i)
  {
    PrintVertex(cell.vertices[i], stream, depth + 2);
    stream << ",\n";
  }
  PrintVertex(cell.vertices[i], stream, depth + 2);
  stream << "\n" << std::string(depth + 1, '\t') << "]\n" << std::string(depth, '\t') << "}";
}
void voronoi::JsonSerialize(CellArray const & cells, std::ostream & stream)
{
  int i = 0;
  stream << "\"cells\" : {\n";
  for(; i < cells.size() - 1; ++i)
  {
    stream << "\t\"" << i << "\": ";
    PrintCell(cells[i], stream, 1);
    stream << ",\n";
  }
  stream << "\t\"" << i << "\": ";
  PrintCell(cells[i], stream, 1);
  stream << "\n}";
}