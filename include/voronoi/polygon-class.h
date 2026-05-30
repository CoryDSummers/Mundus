#pragma once
#include "voronoi/types.h"
namespace voronoi
{
  class Polygon
  {
    public:
      Polygon();
      bool Inside(Vertex point);
      VertexArray & Data() { return vertice_array_; }
    private:
      VertexArray vertice_array_;
  };
}