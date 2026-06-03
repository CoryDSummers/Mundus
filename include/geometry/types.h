#ifndef MUNDUS_GRAPHICS_TYPES_H_
#define MUNDUS_GRAPHICS_TYPES_H_
#include <glm/vec2.hpp>
#include <vector>
namespace mundus::geometry
{
  using Vertex = glm::vec2;
  using VertexArray = std::vector<Vertex>;
}

#endif //MUNDUS_GRAPHICS_TYPES_H_