#ifndef MUNDUS_GEOMETRY_GEOMETRY_POOL_H_
#define MUNDUS_GEOMETRY_GEOMETRY_POOL_H_
#include "geometry/types.h"

namespace mundus::geometry
{
  class GeometryPool
  {
    public:
      GeometryPool();
      Vertex & operator[](std::size_t index) { return global_vertices_[index]; }
    private:
      VertexArray global_vertices_;
  };
}

#endif //MUNDUS_GEOMETRY_GEOMETRY_POOL_H_