#include "voronoi/polygon-class.h"
#include "glm/mat2x2.hpp"

inline bool TriangleIsCCW(const voronoi::Vertex & a, const voronoi::Vertex & b, const voronoi::Vertex & c) {
    float determinant = static_cast<float>(((a.x - c.x) * (b.y - c.y)) - ((a.y - c.y) * (b.x - c.x)));
    return determinant > 0.0f;
}

bool voronoi::Polygon::Inside(Vertex point)
{
  const int k_vertice_count = static_cast<int>(this->vertice_array_.size());
  if(k_vertice_count < 3)
  {
    return false;
  }
  int low = 0;
  int high = k_vertice_count;
  const glm::vec2 v0 = vertice_array_[0];
  do {
    int mid = (low + high) / 2;
    const glm::vec2 vMid = vertice_array_[mid];
    if(TriangleIsCCW(vertice_array_[0], vertice_array_[mid], point))
    {
      low = mid;
    }
    else{
      high = mid;
    }
  } while (low + 1 < high);
  if(low == 0 || high == k_vertice_count)
  {
    return false;
  }
  const glm::vec2 vLow = vertice_array_[low];
  const glm::vec2 vHigh = vertice_array_[high];
  return TriangleIsCCW(vertice_array_[low], vertice_array_[high], point);
}